#include <ncurses.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "utils.h"
#include "ui.h"

void draw_box_around_message_box(int x, int y) {
	for (int i = BORDER_SIZE; i < x; i++) 
		mvprintw(y-2, i, "#"); 
	
	for (int i = 0; i < y+1; i++)
		mvprintw(i, BORDER_SIZE, "#");
	
	mvprintw(y-2, x-13, "Scroll: %d/%d", scroll_, max_scroll);

	refresh();
}

void clear_message_box(int max_x, int max_y) {
	for (int i = BORDER_SIZE+2; i < max_x; i++) {
		mvprintw(max_y-2, i, " "); // apparently doesn't clear fucking shit
		mvprintw(max_y-1, i, " ");
	}
	mvprintw(max_y-2, BORDER_SIZE+2, ">> ");
	refresh();
}


void del_char(int x, int y, char* buf) {
	clear_message_box(x, y);
	printw("%s", buf);
}

void clear_all_messages(int x, int y) {
	for (int jy = 0; jy < y-3; jy++) {
		for (int ix = BORDER_SIZE+1; ix < x; ix++) {
			mvprintw(jy, ix, " ");
		}
	}

	refresh();
}

void draw_users(TWOD_CHAR_ARR users) {
	clear_user_box();
	int counter = 0;
	
	for (size_t i = 0; i < BORDER_SIZE-1; i++) 
		mvprintw(1, i, "=");
	

	for (size_t i = 0; i < 50; i++) {
		if (users.user_arr[i][0] == '\0') 
			break;

		mvprintw(i+2, 0, "%s",  users.user_arr[i]);
		counter++;
	}

	mvprintw(0, 0, "Users: %d", counter);
	move(y_draw-2, BORDER_SIZE + msg_counter + 5); // subject to change as soon as message wrappability is fixed.

	refresh();
}

TWOD_CHAR_ARR get_user_array(char* users) {
	TWOD_CHAR_ARR arr;
	memset(&arr, '\0', 50*18);
	
	size_t counter = 0;
	size_t user_arr_counter = 0;
	char buffer[18];

	for (size_t i = 0; i < strlen(users); i++) {
		if (users[i] == ';') {
			strcpy(arr.user_arr[user_arr_counter++], buffer);
			memset(buffer, '\0', 18);
			counter = 0;
			continue;
		}
		buffer[counter] = users[i];
		counter++;
	}
	
	return arr;
}

void draw_server_message(int y, struct MsgNode* h) {
	if (scroll_ >= 1) {
		return;
	}
	int msg_len = strlen(h->msg);

	int msg_start = floor(SERVER_NOTIF_WHITESPACE/2) - floor(msg_len/2);

	move(y, BORDER_SIZE+2);

	for (int i = 0; i < SERVER_REPETITION; i++)
		printw("%c", SERVER_NOTIFICATION_FRONT);
	
	for (int i = 0; i < msg_start-1; i++)
		printw(" ");

	printw("%s", h->msg);
	int left = SERVER_NOTIF_WHITESPACE - msg_start - 1 - msg_len;
	
	for (int i = 0; i < left; i++)
		printw(" ");
	
	for (int i = 0; i < SERVER_REPETITION; i++)
		printw("%c", SERVER_NOTIFICATION_BACK);
}

void draw_messages(int y, struct MsgNode* h, unsigned int scroll) {
	if (strcmp(h->sentby, "SERVER") == 0) {
		draw_server_message(y, h);
		goto next;
	}

	char full_buffer[2050] = { '\0' };
	strcat(full_buffer, h->sentby);
	strcat(full_buffer, ": ");
	strcat(full_buffer, h->msg);
	
	int temp_max_scroll = floor(strlen(full_buffer) / max_message_length);
	if (temp_max_scroll > max_scroll) 
		max_scroll = temp_max_scroll;
	
	if (scroll * max_message_length > strlen(full_buffer))
		goto next;

	char* send_buffer = (char*) malloc(max_message_length);

	if (send_buffer == NULL) {
		mvprintw(y, BORDER_SIZE+2, "failed to load in message...");
		goto next;
	}

	memset(send_buffer, '\0', max_message_length);
	string_slice(full_buffer, send_buffer, scroll * max_message_length, max_message_length + (max_message_length * scroll));
	mvprintw(y, BORDER_SIZE+2, "%s", send_buffer);
	free(send_buffer);
	send_buffer = NULL;
	
next:
	if (h->next != NULL) {
		draw_messages(y+1, h->next, scroll);
		return;
	}

	int ax, ay;
	getmaxyx(stdscr, ay, ax);	
	draw_box_around_message_box(ax, ay-1);
	refresh();
}


void clear_user_box() {
	for (int x = 0; x < BORDER_SIZE; x++) {
		for (int y = 0; y < y_draw; y++) {
			mvprintw(y, x, " ");
		}
	}

	refresh();
}
