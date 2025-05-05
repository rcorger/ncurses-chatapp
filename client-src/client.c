#include <sys/socket.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

#include "config.h"
#include "utils.h"
#include "ui.h"
/*
	TODO:
		FINISH UI
			- end program upon death of server
*/
// 3 border
// 3 per msg

struct MsgNode* head;
static bool RUNNING;
const char* user_loop_msg = "/users";

//int handle_args(int argc, char** argv) {
//	// TODO 
//	return -1;
//}

void* users_send_loop(void* server_fd) {
	while (RUNNING) {
		send(server_fd, user_loop_msg, strlen(user_loop_msg), 0);
		sleep(WAIT_TIME);
	}
	pthread_exit(NULL);
}

void* read_loop(void* server_fd) {
	char buf[2048];
	int return_code;

	while (RUNNING) {
		memset(buf, '\0', sizeof(buf));
		return_code = recv((int)server_fd, buf, 2048, 0);
		if (return_code == 0) {
			close((int)server_fd);
			break;
		}

		// successfully received some kind of message;	
		char lemsg[2048];
		char uname[18];

		memset(lemsg, '\0', 2048*sizeof(char));
		memset(uname, '\0', 18*sizeof(char));
		
		bool found_uname = false;
		size_t counter = 0;

		for (size_t i = 0; i < 2048; i++) {
			if (buf[i] == '\0') 
				break;
	
			if (buf[i] == ':' && found_uname == false) {
				found_uname = true;
				counter = 0;
				continue;
			}

			if (found_uname) 
				lemsg[counter++] = buf[i];

			else if (!found_uname) 
				uname[counter++] = buf[i];
		}

		if (strcmp(uname, "SERVER") == 0) {
			bool users = false;
			for (int i = 0; i < 2048; i++) {
				if (lemsg[i] == '\0')
					break;
				if (lemsg[i] == ';') {
					users = true;
					break;
				}
			}
			
			if (users == true) {
				TWOD_CHAR_ARR users = get_user_array(lemsg);
				draw_users(users);
				continue;
			}
		}
		
		list_append(&head, lemsg, uname);
		check_for_too_many_messages(&head);
		
		int ax,ay;
		getmaxyx(stdscr, ay, ax);
	
		clear_all_messages(ax, ay);
		draw_messages(0, head, scroll_);

	    move(y_draw-2, BORDER_SIZE + 5 + msg_counter);
		refresh();
	}
	pthread_exit(NULL);	
}

int main(int argc, char** argv) {
	(void) argc;
	(void) argv;
	// CONNECT TO SOCKET/SERVER...
	
	RUNNING = true;
	printf("Connecting to server...\n");	
	int server_fd;
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("well fuck, it didn't work.\n");
		exit(1);
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);	

	if (inet_pton(AF_INET, ADDRESS_CONNECTING, &serv_addr.sin_addr) != 1) {
		printf("Failed misserably at int_pton stage.\n");
		goto error;
	}		
	
	if (connect(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
		printf("Fuck me, connect didn't work.\n");
		goto error;
	}	

		
	// FROM HEREIN WE CAN ASSUME WE CONNECTED SUCCESSFULLY
	

	char handshake[19];
	sscanf(USERNAME, "%s", handshake);
	strcat(handshake, ":");
	send(server_fd, handshake, strlen(handshake), 0);
	
	pthread_t new_t;
	pthread_create(&new_t, NULL, read_loop, (int)server_fd);
	
	// ONLY THEN INITIALIZE THE SCREEN 


	// basic ncurses initialization
	initscr(); cbreak(); noecho(); keypad(stdscr, TRUE);
	

	int x, y;
	getmaxyx(stdscr, y, x);
	
	initialize_VARS(y-3);

	draw_box_around_message_box(x, y_draw-1);
	clear_message_box(x, y_draw); // message box && changes the cursor location

	pthread_t users_t;
	pthread_create(&users_t, NULL, users_send_loop, server_fd);
	
	head = initialize_node("You joined the chatroom...", "SERVER");
	draw_messages(0, head, scroll_);

	char buf[2048] = { '\0' };
	// ui stuff
	//getmaxyx(stdscr, y_draw, x_draw);

	int MSG_BOX_Y = y_draw - 2;
	int NORMAL_POS_X = BORDER_SIZE + 5;
	move(MSG_BOX_Y, NORMAL_POS_X);	

	int ax, ay;
	char c;
	for (;;) {
		c = getch();
		
		switch ((int)c) {
			case 4: //LEFT
				if (scroll_ > 0)
					scroll_--;

				getmaxyx(stdscr, ay, ax);
				clear_all_messages(ax, ay);
				draw_messages(0, head, scroll_);
				
			    move(y_draw-2, BORDER_SIZE + 5 + msg_counter);
				break;
			case 5: //RIGHT
				if (scroll_ + 1 <= max_scroll)
					scroll_++;
				getmaxyx(stdscr, ay, ax);
				clear_all_messages(ax, ay);
				draw_messages(0, head, scroll_);
			    move(y_draw-2, BORDER_SIZE + 5 + msg_counter);
				break;
			case 10: //enter
				if (msg_counter == 0)
					break;
		
				getmaxyx(stdscr, ay, ax);

				if (strcmp(buf, "/quit") == 0 || strcmp(buf, "/exit") == 0) {
					close(server_fd);
					endwin();
					RUNNING = false;
					exit(0);
					break;
				}
				else if (strcmp(buf, "/clear") == 0) {
					msg_counter = 0;
					clear_message_box(ax, ay);
					memset(buf, '\0', 2048);

					delete_all_messages(&head);
					clear_all_messages(ax, ay);
					head = initialize_node("Cleared all messages from screen.", "SERVER");
					max_scroll = 0;
					scroll_ = 0;
					draw_messages(0, head, scroll_);
					break;	
				}
			
				send(server_fd, buf, strlen(buf), 0);
				list_append(&head, buf, USERNAME);
				check_for_too_many_messages(&head);

			
				clear_all_messages(ax, ay);
				draw_messages(0, head, scroll_);

				memset(buf, '\0', 2048);
				msg_counter = 0;
				
				clear_message_box(ax, ay);
				break;
			case 7:
				buf[msg_counter] = '\0';
				if (msg_counter > 0) 
					buf[--msg_counter] = '\0';

				getmaxyx(stdscr, ay, ax);
				clear_message_box(ax, ay);
				printw("%s", buf);
				break;
			
			default:
				if (msg_counter >= max_message_length - 5) 
					mvprintw(y_draw-1, BORDER_SIZE + 5 + msg_counter - max_message_length + 5, "%c", c);
				else
					mvprintw(y_draw-2, BORDER_SIZE + 5 + msg_counter, "%c", c);
				
				buf[msg_counter++] = c;
				break;
		}

		refresh();
	}
	endwin();
	return 0;

error:
	close(server_fd);
	return 1;
}
