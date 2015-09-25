#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server.h"

uint MAX_CONNECTIONS = 8;

int main(int argc, char *argv[]) {
	int sockfd, clisockfd;
	struct sockaddr_in cli_addr;
	static const struct sockaddr_in empty_sockaddr_in;
	struct Server server;
	if(argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}
	sockfd = init(&server, atoi(argv[1]), MAX_CONNECTIONS);
	if(sockfd < 0) {
		perror("ERROR opening socket\n");
		exit(1);
	}
	serve(&server);
	while(1) {
		clisockfd = get_client(&server, (struct sockaddr *) &cli_addr);
		if (clisockfd < 0) {
			perror("ERROR on accept\n");
		} else if (clisockfd == 0) {
			fprintf(stdout, "Maximum connections reached\n");
		}
		if (fork() == 0) {
			close(sockfd);
			fprintf(stdout, "Received a connection\n");
			char * addr = inet_ntoa(cli_addr.sin_addr);
			fprintf(stdout, "Peer - %s\n", addr);
			close(clisockfd);
			return 0;
		} else {
			close(clisockfd);
			cli_addr = empty_sockaddr_in;
		}
	}
	close(sockfd);
	return 0;
}