#ifndef _client_h
#define _client_h

#include <stdio.h>
#include <netinet/in.h>

typedef struct Client {
	int sockfd;
	char session_id[26];
	struct sockaddr_in cli_addr;
	int logfd;
	char buf[4096];
} Client;

int client_handle(Client *client, int sockfd, struct sockaddr_in *cli_addr, int logfd);
int client_generate_sessionid(Client *client);
int client_message_ident(Client *client);
int client_message_auth(Client *client);
int client_message_data(Client *client);
int client_read(Client *client, char delim);
int client_write_log(Client *client, char *data);

#endif