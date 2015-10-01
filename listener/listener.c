#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"
#include "client.h"
#include "util.h"

uint MAX_CONNECTIONS = 8;

int main(int argc, char *argv[]) {
	int sockfd, clisockfd;
	struct sockaddr_in cli_addr;
	static const struct sockaddr_in empty_sockaddr_in;
	struct Server server;
	struct Client client;
	if(argc < 2) {
		error("ERROR, no port provided");
	}
	FILE *logfd = fopen("log.txt", "w+");
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
		} 
		if (fork() == 0) {
			close(sockfd);
			client_handle(&client, clisockfd, &cli_addr, logfd);
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