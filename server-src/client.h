#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

#include "config.h"
#include "client.h"

typedef struct {
	bool success;
	char username[18];
	int client_fd;
	unsigned long uuid;
} Client;

short client_arr_size(Client* client_arr[MAX_CLIENTS]);
bool client_pop(Client* client_arr[MAX_CLIENTS], Client* c);
bool client_add(Client* client_arr[MAX_CLIENTS], Client* c);
bool client_exists(Client* client_arr[MAX_CLIENTS], Client* c);
Client* get_client_by_name(Client* client_arr[MAX_CLIENTS], char uname[18]);

#endif // CLIENT_H
