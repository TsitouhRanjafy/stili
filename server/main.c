
#define SERVER_UTIL_IMPLEMENTATION
#include "./server_util.h"

void acceptNewConnectionAndReceiveAnPrintItsData(int socketFD);

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

void acceptNewConnectionAndReceiveAnPrintItsData(int socketFD){
    while(true){
        printf("waiting a client...\n");
        if (acceptedSocketCount < CLIENT_MAX) {
            struct AcceptedSocket * clientSocket = acceptIncomingConnection(socketFD);
            acceptedSocket[acceptedSocketCount++] = *clientSocket;
            printf("client joined[id: %d]\n", (acceptedSocketCount - 1));
            receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
        }
    }
}

