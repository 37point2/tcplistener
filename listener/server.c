#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "server.h"

int init(Server *server, const int portno, uint max_connections) {
	struct sockaddr_in serv_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		perror("ERROR opening socket\n");
		exit(1);
	}
	server->sockfd = sockfd;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	server->serv_addr = serv_addr;
	server->max_connections = max_connections;
	server->current_connections = 0;
	return sockfd;
}

void serve(Server *server) {
	if(bind(server->sockfd, (struct sockaddr *) &server->serv_addr, (socklen_t) sizeof(server->serv_addr)) < 0) {
		perror("ERROR on binding\n");
		exit(1);
	}
	listen(server->sockfd,5);
}

int get_client(Server *server, struct sockaddr *cli_addr) {
	if (server->current_connections > 0)
		cleanup_closed_connections(server);
	if (server->current_connections < server->max_connections) {
		int clilen = sizeof(cli_addr);
		int clisockfd = accept(server->sockfd, (struct sockaddr *) cli_addr, (socklen_t *) &clilen);
		if (clisockfd > 0)
			server->current_connections++;
		return clisockfd;
		}
	return 0;
}

void cleanup_closed_connections(Server *server) {
	int status;
	while(server->current_connections > 0) {
		if (waitpid(-1, &status, WNOHANG) > 0) {
			server->current_connections--;
			fprintf(stdout, "Child exited with status - %d\n", status);
		} else {
			break;
		}
	}
}
