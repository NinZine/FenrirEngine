#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "net.h"

void
net_close(int s)
{

    close(s);
}

uint32_t
net_iptoi(const char *ip)
{

    return inet_addr(ip);
}

int
net_open_udp(uint32_t ip, uint16_t port)
{
    struct sockaddr_in sin;
    int s;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    //fcntl(s, F_SETFL, O_NONBLOCK);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if (0 == ip) {
        sin.sin_addr.s_addr = INADDR_ANY;
    } else {
        sin.sin_addr.s_addr = ip;
    }

    if (0 != port && 0 != bind(s, (struct sockaddr *)&sin, sizeof(sin))) {
        printf("net> could not bind socket\n");
    }
	
	return s;
}

char*
net_recv(int s)
{
    struct sockaddr_in sin;
    int bytes;
    socklen_t sin_len;
    char *str = 0;

    sin_len = sizeof(sin);
    str = malloc(1024 * sizeof(char));
    bytes = recvfrom(s, str, 1024, 0, (struct sockaddr *)&sin, &sin_len);

    if (-1 == bytes) {
        printf("net> failed to recieve");
        return 0;
    } else if (0 == bytes) {
        return 0;
    }
    printf("recieved %d bytes from %s\n", bytes, inet_ntoa(sin.sin_addr));


    return str;
}

uint32_t
net_resolve_host(const char *hostname)
{
    struct sockaddr_in *s;

    if (0 != getaddrinfo(hostname, 0, 0, (struct addrinfo **)&s)) {
        printf("net> could not resolve %s\n", hostname);
    }

    return s->sin_addr.s_addr;
}

void
net_send(int socket, const char *msg, uint32_t ip, uint16_t port)
{
    struct sockaddr_in sin;
    int bytes;

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = ip;

    bytes = sendto(socket, msg, strlen(msg)+1, 0, (struct sockaddr *)&sin,
        sizeof(sin));
    if (-1 == bytes) {
        printf("net> failed to send.");
    } else {
        printf("sent %d bytes to %s\n", bytes, inet_ntoa(sin.sin_addr));
    }
}

