#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "config.h"


Client* get_client_by_name(Client* client_arr[MAX_CLIENTS], char uname[18]) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (client_arr[i] != NULL) {
			if (strcmp(client_arr[i]->username, uname) == 0) {
				return client_arr[i];
			}  
		}
	}
	return NULL;
}

short client_arr_size(Client* client_arr[MAX_CLIENTS]) {
	short counter = 0;
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (client_arr[i] != NULL) {
			counter += 1;
		}
	}
	return counter;
}

bool client_pop(Client* client_arr[MAX_CLIENTS], Client* c) {
	//error occurs here @ strcmp
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (client_arr[i] == NULL)
			continue;
	
		if (client_arr[i]->uuid == c->uuid) {
			close(c->client_fd);
			free(c);
			client_arr[i] = NULL;
			return true;
		}
	}
	return false;
}

bool client_add(Client* client_arr[MAX_CLIENTS], Client* c) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (client_arr[i] == NULL) {
			client_arr[i] = c;
			return true;
		}
	}	
	return false;
}

bool client_exists(Client* client_arr[MAX_CLIENTS], Client* c) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (client_arr[i] != NULL) {
			if (strcmp(client_arr[i]->username, c->username) == 0) {
				return true; // username duplicate.
			} 
		}
	}
	return false;
}
