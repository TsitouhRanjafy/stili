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

    char *line = NULL;
    size_t lineSize = 0;
    printf(" send a message (type exit())\n");
    while(1) {
        printf("\tTsitohaina: ");
        size_t charCount = getline(&line, &lineSize, stdin);
        if (strcmp(line, "exit()\n") == 0) {
            printf("\n Bey Tsitohaina\n");
            break;
        }
        send(socketFD, line, charCount, 0);
    }

        
    free(line);
    close(socketFD);
    free(address);
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