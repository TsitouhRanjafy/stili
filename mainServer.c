#include <errno.h>
#include <unistd.h>

#define SOCKETUTIL_IMPLEMENTATION
#include "lib/socketutil.h"


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

    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD,(struct sockaddr *) &clientAddress, &clientAddressSize);


    char buffer[1024];
    while(1){
        ssize_t amountReceved = recv(clientSocketFD, buffer, 1024, 0);
        if(amountReceved > 0)
            printf("unknow client: %s", buffer);
        if (amountReceved < 0)
            break;
        memset(buffer, 0, sizeof(buffer));
    }

    close(clientSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    return EXIT_SUCCESS;
}


