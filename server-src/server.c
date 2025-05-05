#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "messages.h"
#include "client.h"

/*
TODO:
	VERIFICATION BY IP <------ NEEDED AT LATER DATE
*/

Client* clients[MAX_CLIENTS] = { NULL };
unsigned long uuid_max = 0;

void parser(Client* user, char* init) {
	short successes = 0; // prone to errors but its alr for now
	char smallbuf[18] = {0};
	int current = 0;

	user->uuid = ++uuid_max;

	if (strlen(init) > 19) {
		user->success = false;
		return;
	}
	for (int i = 0; i < strlen(init); i++) {
		if (init[i] == ':') {
			current = 0;
			successes++;
			
			if (successes == 1) {
				strcpy(user->username, smallbuf);
			} else if (successes > 4) {
				user->success = false;
				return;
			}

			memset(smallbuf, '\0', sizeof(smallbuf));
		}

		smallbuf[current] = init[i];
		current++;
	}

	if (strcmp("SERVER", user->username) == 0) {
		user->success = false;
		return;
	}

	if (client_exists(clients, user)) {
		user->success = false;
		return;
	}
	
	if (successes == 0 || successes >= 2)
		user->success = false;
	else
		user->success = true;
}

void* accept_client(void* client_fd) {
	//int client_fd = *(int*)arg; // converts void* to int* and then dereferences it.
	// initialization of user via parsing of string -> UNAME:
	if (client_arr_size(clients) == MAX_CLIENTS) {
		printf("Can't accept connection as I already have too many.\n");
		close((int)client_fd);
		pthread_exit(NULL);
	}
	
	Client* c = malloc(sizeof(Client));	
	c->client_fd = (int)client_fd;

	char buf[2048] = {0};
	recv(c->client_fd, buf, 2048, 0); // -> this is the "handshake"

	parser(c, buf);	

	if (c->success == false) {
		printf("%s has failed verification.\n", c->username);
		close(c->client_fd);
		free(c);
		pthread_exit(NULL);
	}

	if (client_add(clients, c) == false) {
		printf("%s has failed to be connected due to amount of users in chat already.", c->username);
		close(c->client_fd);
		free(c);
		pthread_exit(NULL);
	}

	join_message(clients, c->username);

	printf("User %s has connected to the chat room.\n", c->username);

	for (;;) {
		memset(buf, '\0', sizeof(buf));
		int return_code = recv(c->client_fd, buf, 2048, 0);	
	
		// REMOVE ERORR_COUNT
		printf("%d\n", return_code);
		if (return_code == 0) {		
			printf("%s has disconnected.\n", c->username);
			char buf[2048] = { 0 };
			sprintf(buf, "%s has disconnected", c->username);
			client_pop(clients, c);
			broadcast_message(clients, buf, "SERVER");
			pthread_exit(NULL);
			continue;
		}
		if (strcmp(buf, "/users") == 0) {
			char umsers[2048] = { 0 };
			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (clients[i] != NULL) {	
					strcat(umsers, clients[i]->username);
					strcat(umsers, ";");
				}

			}
			printf("Sending: %s\n", umsers);
			send_users(c, umsers);
			memset(umsers, '\0', 2048*sizeof(char));
			continue;
		}
		
		broadcast_message(clients, buf, c->username);
		printf("'%s' said: %s\n", c->username, buf);
	}
}

int main() {
	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);	

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		printf("Failed to open socket; returned with error code 1.\n");
		return 1;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) != 0) {
		printf("Failed to bind socket; returned with error code 2.\n");
		return 2;
	}


	if (listen(server_fd, MAX_BACKLOG) != 0) {
		printf("Couldn't accept client.\n");
		return 3;
	}
	
	
	printf("Listening on port: %d\n", PORT);
	int new_socket;

	for (;;) {
		if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
			printf("Failed to accept connection. Exitting.\n");
			close(server_fd);
			return 1;
		}
		
		pthread_t new_t;
		pthread_create(&new_t, NULL, accept_client, (void*)new_socket);
	}

	return 0;
}
