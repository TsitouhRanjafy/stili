#include <errno.h>

#define SOCKETUTIL_IMPLEMENTATION
#include "../shared/socketutil.h"
#include <unistd.h>


int main() {
    const int socketFD = createTCPIpv4Socket();
    char *ip = "127.0.0.1";
    struct sockaddr_in *address = createIPv4Address(ip, 2000);

    int result = connect(socketFD, (struct sockaddr *)address, sizeof(*address));
    if (result != 0) {
        printf("connexion failed [%d]: %s\n", result, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf(" connected with success\n");


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

    // char buffer[1042];
    // recv(socketFD, buffer, 1024, 0);

    // printf("response: %s\n", buffer);

    close(socketFD);
    free(address);
    free(line);
    return EXIT_SUCCESS;
}
