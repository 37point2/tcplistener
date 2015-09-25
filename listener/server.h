#ifndef _server_h
#define _server_h

#include <sys/socket.h>

typedef struct Server {
	int sockfd;
	struct sockaddr_in serv_addr;
	uint max_connections;
	uint current_connections;
} Server;

int init(Server *server, const int portno, uint max_connections);
void serve(Server *server);
int get_client(Server *server, struct sockaddr *cli_addr);
void cleanup_closed_connections(Server *server);

#endif