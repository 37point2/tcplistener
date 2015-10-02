#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"
#include "util.h"

const char *identack = "IDENTACK";
const char *authack = "AUTHACK";
const char *dataack = "DATAACK";

int client_handle(Client *client, int sockfd, struct sockaddr_in *cli_addr, int logfd) {
	client->sockfd = sockfd;
	client->cli_addr = *cli_addr;
	client->logfd = logfd;
	memset(client->buf, 0, sizeof(client->buf));
	char * addr = inet_ntoa(cli_addr->sin_addr);
	fprintf(stdout, "Peer - %s:%d\n", addr, cli_addr->sin_port);
	if (client_generate_sessionid(client) != 0) {
		error("Could not generate session id");
	}
	if (client_message_ident(client) != 0) {
		error("Unknown client");
	}
	write(client->sockfd, identack, sizeof(identack));
	if (client_message_auth(client) != 0) {
		error("Unauthorized client");
	}
	write(client->sockfd, authack, sizeof(authack));
	if (client_message_data(client) != 0) {
		error("Error reading data from client");
	}
	write(client->sockfd, dataack, sizeof(dataack));
	return 0;
}

int client_generate_sessionid(Client *client) {
	time_t t = time(NULL);
	uint32_t cliaddr = client->cli_addr.sin_addr.s_addr;
	in_port_t cliport = client->cli_addr.sin_port;
	snprintf(client->session_id, sizeof(client->session_id), "%d%d%d", (int) t, cliaddr, cliport);
	if (client->session_id[0] == '\0') {
		return -1;
	}
	return 0;
}

int client_message_ident(Client *client){
	int pos = client_read(client, '\n');
	if (pos == -1) {
		error("Socket closed");
	}
	// add extra char for null terminator
	char data[pos+1];
	memset(data, 0, sizeof(data));
	strncpy(data, client->buf, pos);
	if (sizeof(data) > 6) {
		if (data[0] == 'I' && 
			data[1] == 'D' &&
			data[2] == 'E' &&
			data[3] == 'N' &&
			data[4] == 'T' &&
			data[5] == '\t') {
			char out[sizeof(client->session_id) + 1 + sizeof(data) + 1];
			snprintf(out, sizeof(out), "%s\t%s\n", client->session_id, data);
			client_write_log(client, out);
			return 0;
		}
	}
	return -1;
}

int client_message_auth(Client *client){
	int pos = client_read(client, '\n');
	if (pos == -1) {
		error("Socket closed");
	}
	// add extra char for null terminator
	char data[pos+1];
	memset(data, 0, sizeof(data));
	strncpy(data, client->buf, pos);
	if (sizeof(data) > 5) {
		if (data[0] == 'A' && 
			data[1] == 'U' &&
			data[2] == 'T' &&
			data[3] == 'H' &&
			data[4] == '\t') {
			char out[sizeof(client->session_id) + 1 + sizeof(data) + 1];
			snprintf(out, sizeof(out), "%s\t%s\n", client->session_id, data);			client_write_log(client, out);
			return 0;
		}
	}
	return -1;
}

int client_message_data(Client *client) {
	int pos = client_read(client, '\n');
	if (pos == -1) {
		error("Socket closed");
	}
	// add extra char for null terminator
	char data[pos+1];
	memset(data, 0, sizeof(data));
	strncpy(data, client->buf, pos);
	if (sizeof(data) > 5) {
		if (data[0] == 'D' && 
			data[1] == 'A' &&
			data[2] == 'T' &&
			data[3] == 'A' &&
			data[4] == '\t') {
			char out[sizeof(client->session_id) + 1 + sizeof(data) + 1];
			snprintf(out, sizeof(out), "%s\t%s\n", client->session_id, data);
			client_write_log(client, out);
			return 0;
		}
	}
	return -1;
}

int client_read(Client *client, char delim) {
	// read until newline character reached
	int nread;
	do {
		nread = read(client->sockfd, client->buf, sizeof(client->buf));
		//nothing read, call interrupted
		if (nread < 0 && errno == EINTR)
			continue;
		if (nread < 0) {
			perror("Error reading");
			exit(1);
		}
		// Socket closed prematurely
		if (nread == 0) {
			return -1;
		}
	} while (strchr(client->buf, delim) == NULL);
	// subtract addresses to get index
	return (int) (strchr(client->buf, delim) - client->buf);
}

int client_write_log(Client *client, char *data) {
	while(1) {
		if (flock(client->logfd, LOCK_EX) == 0) {
			write(client->logfd, data, strlen(data));
			flock(client->logfd, LOCK_UN);
			break;
		} else {
			perror("Could not get lock");
		}
	}
	return 0;
}