/*

    To use this library, do this in *one* C file (strict order):
            #define SOCKETUTIM_IMPLEMENTATION
            #include "lib/socketutil.h"

*/

#ifndef SOCKETUTIL_H
#define SCOKETUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int createTCPIpv4Socket();
struct sockaddr_in *createIPv4Address(char *ip, int port);

#ifdef SOCKETUTIL_IMPLEMENTATION

int createTCPIpv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in *createIPv4Address(char *ip, int port) {
    struct sockaddr_in * address = malloc(sizeof(struct sockaddr_in));
    if (address == NULL) {
        printf("Error on malloc sockaddr in createIpv4Address");
        exit(EXIT_FAILURE);
    }
    address->sin_port = htons(port);
    address->sin_family = AF_INET;

    if (strlen(ip) == 0)
        address->sin_addr.s_addr = INADDR_ANY;
    else 
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);

    return address;
}

#endif
#endif