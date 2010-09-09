#include <sys/fcntl.h>
#include <sys/types.h>
#if defined(WIN32)
# define _WIN32_WINNT 0x0501 /* XP or later */
# include <winsock2.h>
# include <ws2tcpip.h>
# define EWOULDBLOCK WSAEWOULDBLOCK
#else
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#include <errno.h>
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

char*
net_itoip(uint32_t ip)
{
	struct in_addr in;

	in.s_addr = ip;

    return inet_ntoa(in);
}

/* Open an TCP socket. Set ip to 0 to listen, port to 0 for next available. */
int
net_open_tcp(uint32_t ip, uint16_t port)
{
	struct sockaddr_in sin;
	int s;

	if (-1 == (s = socket(PF_INET, SOCK_STREAM, 0))) {
		return s;
	}

/*#if defined(WIN32)
    u_long flags;
    ioctlsocket(s, FIONBIO, &flags);
#else
    fcntl(s, F_SETFL, O_NONBLOCK);
#endif*/

    if (0 == ip) {
		/* Listen */
        sin.sin_addr.s_addr = INADDR_ANY;
	} else {
		/* Connect */
		struct sockaddr_in server;
		/*sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
		if (0 != bind(s, (struct sockaddr *)&sin, sizeof(sin))) {
			printf("net> could not bind socket\n");
		}*/

		printf("net> connecting to %s\n", net_itoip(ip));
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = ip;
		server.sin_port = htons(port);
		memset(server.sin_zero, '\0', sizeof(server.sin_zero));
		if (0 != connect(s, (struct sockaddr *)&server, sizeof(server))) {
			printf("net> could not connect. %s\n", strerror(errno));
			close(s);
			s = -1;
		}
    }

	return s;
}

/* Open an UDP socket. Set ip to 0 to listen, port to 0 for next available. */
int
net_open_udp(uint32_t ip, uint16_t port)
{
    struct sockaddr_in sin;
    int s;

    s = socket(AF_INET, SOCK_DGRAM, 0);
#if defined(WIN32)
    u_long flags;
    ioctlsocket(s, FIONBIO, &flags);
#else
    fcntl(s, F_SETFL, O_NONBLOCK);
#endif

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

packet
net_recv(int s)
{
    struct sockaddr_in sin;
    int bytes;
    uint32_t sin_len;
    /*static char str[1024];*/ /* Every packet uses this same string. */
	char *str = 0;
	packet p;

	memset(&p, sizeof(packet), 0);
    sin_len = sizeof(sin);
    str = malloc(1024 * sizeof(char)); /* TODO: Potential leak in Lua? */
	str[0] = 0;
    bytes = recvfrom(s, str, 1024, 0, (struct sockaddr *)&sin, &sin_len);
	p.ip = sin.sin_addr.s_addr;
	p.port = ntohs(sin.sin_port);
	p.msg = str;

    if (-1 == bytes) {
		if (EWOULDBLOCK != errno)
        	printf("net> failed to recieve. %s\n", strerror(errno));
        
    } else if (0 == bytes) {
    } else {
    	//printf("recieved %d bytes from %s\n", bytes, inet_ntoa(sin.sin_addr));
	}

    return p;
}

uint32_t
net_resolve_host(const char *hostname)
{
    struct addrinfo *s;

    if (0 != getaddrinfo(hostname, NULL, NULL, &s)) {
        printf("net> could not resolve %s\n", hostname);
    	return 0;
	}

	return ((struct sockaddr_in *)s->ai_addr)->sin_addr.s_addr;
}

void
net_send(int socket, const char *msg, uint32_t ip, uint16_t port)
{
    struct sockaddr_in sin;
    int bytes;

	if (ip) {
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
		sin.sin_addr.s_addr = ip;

		bytes = sendto(socket, msg, strlen(msg)+1, 0, (struct sockaddr *)&sin,
			sizeof(sin));
		if (-1 == bytes) {
			printf("net> failed sendto. %s\n", strerror(errno));
		} else {
			//printf("sent %d bytes to %s\n", bytes, inet_ntoa(sin.sin_addr));
		}
	} else {
		bytes = send(socket, msg, strlen(msg)+1, 0);
		if (-1 == bytes) {
			printf("net> failed to send. %s\n", strerror(errno));
		}
	}
}

