#ifndef MESSAGES_H
#define MESSAGES_H

#include "config.h"
#include "client.h"

void broadcast_message(Client* client_arr[MAX_CLIENTS], char msg[2030], char uname[18]);
short private_message(char msg[2030], char from[18], char to[18], Client* clients[MAX_CLIENTS]);
void join_message(Client* client_arr[MAX_CLIENTS], char uname[18]);
void send_users(Client* client, char* users);

#endif // MESSAGES_H
