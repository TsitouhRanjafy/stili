#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#define SOCKETUTIL_IMPLEMENTATION
#include "../shared/socketutil.h"
#include <unistd.h>

void * listenAndPrint(void * args);
void startListenAndPrintDataOnNewThread(int socketFD);

int main() {
    int socketFD = createTCPIpv4Socket();
    char *ip = "127.0.0.1";
    struct sockaddr_in *address = createIPv4Address(ip, 2000);

    int result = connect(socketFD, (struct sockaddr *)address, sizeof(*address));
    if (result != 0) {
        printf("connexion failed [%d]: %s\n", result, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf(" connected with success\n");

    
    startListenAndPrintDataOnNewThread(socketFD);
    
    /* setup get name */
    char name[25];
    printf(" Enter your name[%zu]: ",sizeof(name)); scanf("%s",name);
    system("clear");
    
    /* start chating */
    char *message = NULL;
    size_t messageSize = 0;
    size_t buffer = 1023;
    char data[buffer];

    printf(" Welcom to stili %s\n",name);
start_chting:
    printf(" Send a message (type exit(), clear())\n");
    while(getchar() != '\n'){};
    while(1) {
        printf("\t%s: ",name); 
        getline(&message, &messageSize, stdin);
        if (strcmp(message, "exit()\n") == 0) {
            printf("\n Bey %s\n",name);
            break;
        }
        if (strcmp(message, "clear()\n") == 0){
            system("clear");
            goto start_chting;
        } else {
            sprintf(data, "%s: %s", name, message);
            send(socketFD, data, strlen(data), 0);
        }
        
    }

    
    close(socketFD);
    free(address);
    if(message)
        free(message);
    
    return EXIT_SUCCESS;
}

void startListenAndPrintDataOnNewThread(int socketFD){
    pthread_t id;
    int * socketFD_ptr = malloc(sizeof(int));
    if(!socketFD_ptr) {
        printf("malloc failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    *socketFD_ptr = socketFD;

    pthread_create(&id, NULL, listenAndPrint, socketFD_ptr);
    pthread_detach(id);
}

void * listenAndPrint(void * args){
    int socketFD = *(int *) args;
    char buffer[1024];

    while(1){
        ssize_t amountReceved = recv(socketFD, buffer, 1024, 0);
        if(amountReceved > 0) {
            buffer[amountReceved] = 0;
            printf("\n\tresponse: %s", buffer);
        }
        if (amountReceved <= 0)
            break;
    }
}