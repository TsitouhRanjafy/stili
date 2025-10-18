#ifndef SERVER_UTIL
#define SERVER_UTIL

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#define SOCKETUTIL_IMPLEMENTATION
#include "../shared/socketutil.h"
#define CLIENT_MAX 10

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD);
void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket * pSocket);
void * receiveAndPrintIncomingData(void * args);
void sendReceivedDataToOtherClient(int socketFD, char * buffer, size_t buffer_size);

struct AcceptedSocket {
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool isAcceptedSuccessfully;
};

struct AcceptedSocket acceptedSocket[CLIENT_MAX];
int acceptedSocketCount = 0;


#ifdef SERVER_UTIL_IMPLEMENTATION

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
    return args;
}

void sendReceivedDataToOtherClient(int socketFD, char * buffer, size_t buffer_size){
    for (int i = 0; i < acceptedSocketCount; i++) {
        if (acceptedSocket[i].acceptedSocketFD == socketFD) 
            continue;
        send(acceptedSocket[i].acceptedSocketFD, buffer, buffer_size, 0);
    }
}

#endif
#endif
