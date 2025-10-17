#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define SOCKETUTIL_IMPLEMENTATION
#include "lib/socketutil.h"

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD);
void * receiveAndPrintIncomingData(void * args);
void startAcceptIncomingConnection(int socketFD);
void acceptNewConnectionAndReceiveAnPrintItsData(int socketFD);
void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket * pSocket);

struct AcceptedSocket {
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool isAcceptedSuccessfully;
};

void startAcceptIncomingConnection(int socketFD) {
    while(true){
        struct AcceptedSocket * clientSocket = acceptIncomingConnection(socketFD);
        receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
    }
}

void acceptNewConnectionAndReceiveAnPrintItsData(int socketFD){
    while(true){
        struct AcceptedSocket * clientSocket = acceptIncomingConnection(socketFD);
        receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
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

    while(1){
        ssize_t amountReceved = recv(socketFD, buffer, 1024, 0);
        if(amountReceved > 0) {
            buffer[amountReceved] = 0;
            printf("unknow client [%zd]: %s", amountReceved, buffer);
        }
        if (amountReceved <= 0)
            break;
    }

    close(socketFD);
    free(pSocket);
}


int main(){
    const int serverSocketFD = createTCPIpv4Socket();
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

    startAcceptIncomingConnection(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    return EXIT_SUCCESS;
}
