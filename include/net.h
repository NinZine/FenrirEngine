#ifndef _NET_H_
#define _NET_H_

#include <stdint.h>

typedef struct packet {
	uint32_t 	ip;
	uint16_t	port;
	char 		*msg;
} packet;

void        net_close(int32_t socket);
void		net_free(packet *p);
uint32_t    net_iptoi(const char *ip);
char*		net_itoip(uint32_t ip);
int32_t		net_open_tcp(uint32_t ip, uint16_t port);
int32_t		net_open_udp(uint32_t ip, uint16_t port);
packet      net_recv(int32_t socket);
uint32_t    net_resolve_host(const char *hostname);
void        net_send(int32_t socket, const char *msg,
				uint32_t ip, uint16_t port);

#endif

