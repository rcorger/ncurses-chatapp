#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "config.h"
#include "messages.h"

void broadcast_message(Client* client_arr[MAX_CLIENTS], char msg[2030], char uname[18]) {
	if ((strlen(uname) + strlen(msg) + 1) > MAX_BUFF) {
		printf("LOG: User sent a message that was too large!!!\n");
		//tell user their message was too large...
		return;
	}

	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (client_arr[i] != NULL) {
			if (strcmp(client_arr[i]->username, uname) == 0) {
				continue;
			}
			char message[2048];
			strcpy(message, uname);
			strcat(message, ":");
			strcat(message, msg);
			send(client_arr[i]->client_fd, message, strlen(message), 0);
		}
	}
}

void join_message(Client* client_arr[MAX_CLIENTS], char uname[18]) {
		for (int i = 0; i < MAX_CLIENTS; i++) {
		if (client_arr[i] != NULL) {
			if (strcmp(client_arr[i]->username, uname) == 0) {
				continue;
			}

			char message[2048];
			strcpy(message, "SERVER");
			strcat(message, ":");
			strcat(message, uname);
			strcat(message, " has joined the server!");
			send(client_arr[i]->client_fd, message, strlen(message), 0);
		}
	}

}

void send_users(Client* client, char* users) {
	if (client == NULL) 
		return;
	
	char buf[2048] = { 0 };
	if (sprintf(buf, "SERVER:%s", users) < 0)
		return;

	send(client->client_fd, buf, strlen(buf), 0);
}

short private_message(char msg[2030], char from[18], char to[18], Client* clients[MAX_CLIENTS]) {
	// MESSAGE SEND ERROR
	Client* c = get_client_by_name(clients, to);
	if (c == NULL) {
		return -1;
	}

	char buf[2048];
	if (sprintf(buf, "(secr) %s: %s", from, msg) < 0) {
		return -2; // buf was overflown.
	}

	send(c->client_fd, buf, strlen(buf), 0);
	return 0;
}
