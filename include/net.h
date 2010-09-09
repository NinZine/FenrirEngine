#ifndef _NET_H_
#define _NET_H_

#include <stdint.h>

typedef struct packet {
	uint32_t 	ip;
	uint16_t	port;
	char 		*msg;
} packet;

void        net_close(int socket);
uint32_t    net_iptoi(const char *ip);
char*		net_itoip(uint32_t ip);
int			net_open_tcp(uint32_t ip, uint16_t port);
int         net_open_udp(uint32_t ip, uint16_t port);
packet      net_recv(int socket);
uint32_t    net_resolve_host(const char *hostname);
void        net_send(int socket, const char *msg, uint32_t ip, uint16_t port);

#endif

