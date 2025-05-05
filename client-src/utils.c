#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ncurses.h>

#include "utils.h"
#include "config.h"

int MAX_MESSAGES;
int x_draw, y_draw;
int scroll_;
int max_scroll;
int max_message_length;
int msg_counter;

void initialize_VARS(int MM) {
	MAX_MESSAGES = MM;
	msg_counter = 0;
	scroll_ = 0;
	max_scroll = scroll_;

	getmaxyx(stdscr, y_draw, x_draw);
	max_message_length = x_draw - BORDER_SIZE - 4; 
	//printf("Max messages: %d\n", MAX_MESSAGES);
}

struct MsgNode* initialize_node(char msg[2048], char sentby[18]) {
	struct MsgNode* n = (struct MsgNode*)calloc(1, sizeof(struct MsgNode));
	//memset(n, 0, sizeof(struct MsgNode));
	strcpy(n->msg, msg);
	strcpy(n->sentby, sentby);
	return n;
}

void list_append(struct MsgNode** h, char msg[2048], char sentby[18]) {
	struct MsgNode* curr = *h;
	while (curr->next != NULL) 
		curr = curr->next;

	curr->next = (struct MsgNode*) malloc(sizeof(struct MsgNode));
	if (curr->next == NULL) 
		return;

	strcpy(curr->next->msg, msg);
	strcpy(curr->next->sentby, sentby);
	curr->next->next = NULL;
}

void check_for_too_many_messages(struct MsgNode** head_a) {
	int msg_counter = 0;
	struct MsgNode* curr = *head_a;

	while (curr != NULL) {
		curr = curr->next;
		msg_counter++;
	}

	if (msg_counter > MAX_MESSAGES) {
		struct MsgNode* temp = *head_a;
		*head_a = temp->next;
		free(temp);
	}
}

void delete_all_messages(struct MsgNode** head) {
	struct MsgNode* current = *head;
	struct MsgNode* nn;
	
	while (current != NULL) {
		nn = current->next;
		free(current);
		current = nn;
	}

	*head = NULL;
}

void string_slice(const char* str, char* buf, size_t start, size_t end) {
	size_t j = 0;
	for (size_t i = start; i <= end; i++) {
		if (str[i] != '\0')
			buf[j++] = str[i];
		else
			break;
	}
	buf[j] = '\0';
}

