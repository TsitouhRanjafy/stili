#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define SOCKETUTIL_IMPLEMENTATION
#include "../shared/socketutil.h"

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD);
void * receiveAndPrintIncomingData(void * args);
void acceptNewConnectionAndReceiveAnPrintItsData(int socketFD);
void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket * pSocket);
void sendReceivedDataToOtherClient(int socketFD, char * buffer, size_t buffer_size);

struct AcceptedSocket {
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool isAcceptedSuccessfully;
};

struct AcceptedSocket acceptedSocket[10];
int acceptedSocketCount = 0;


void acceptNewConnectionAndReceiveAnPrintItsData(int socketFD){
    while(true){
        printf("waiting a client...\n");
        if (acceptedSocketCount < 10) {
            struct AcceptedSocket * clientSocket = acceptIncomingConnection(socketFD);
            acceptedSocket[acceptedSocketCount++] = *clientSocket;
            printf("client joined[id: %d]\n", (acceptedSocketCount - 1));
            receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
        }
    }
}

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD,(struct sockaddr *) &clientAddress, &clientAddressSize);

    struct AcceptedSocket * acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    if  (acceptedSocket == NULL) {
        printf("Error on malloc struct acceptedSocket");
        exit(EXIT_FAILURE);
    }
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->isAcceptedSuccessfully = clientSocketFD > 0;
    if(!acceptedSocket->isAcceptedSuccessfully)
        acceptedSocket->error = clientSocketFD;

    return acceptedSocket;
}

void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket * pSocket){
    pthread_t id;
    pthread_create(&id, NULL, receiveAndPrintIncomingData, pSocket);
    pthread_detach(id);
}

void * receiveAndPrintIncomingData(void * args) {
    struct AcceptedSocket * pSocket = (struct AcceptedSocket *) args;
    char buffer[1024];
    int socketFD = pSocket->acceptedSocketFD;

    char * a = "v";
    while(1){
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);
        if(amountReceived > 0) {
            buffer[amountReceived] = 0;
            printf("    message[%zd]: %s", amountReceived, buffer);
            sendReceivedDataToOtherClient(socketFD, buffer, amountReceived);
        }
        if (amountReceived <= 0)
            break;
    }

    close(socketFD);
    free(pSocket);
}

void sendReceivedDataToOtherClient(int socketFD, char * buffer, size_t buffer_size){
    for (int i = 0; i < acceptedSocketCount; i++) {
        if (acceptedSocket[i].acceptedSocketFD == socketFD) 
            continue;
        send(acceptedSocket[i].acceptedSocketFD, buffer, buffer_size, 0);
    }
}

int main(){
    int serverSocketFD = createTCPIpv4Socket();
    struct sockaddr_in *serverAddress = createIPv4Address("", 2000);

    int result = bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress));
    if (result != 0) {
        printf("binding failed [%d]: %s\n", result, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("socket was bound successfully\n");

    int listenResult = listen(serverSocketFD, 10);
    if (listenResult != 0) {
        printf("listening failed [%d]: %s", listenResult, strerror(errno));
        exit(EXIT_FAILURE);
    }

    acceptNewConnectionAndReceiveAnPrintItsData(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    printf(" server stoped\n");
    return EXIT_SUCCESS;
}