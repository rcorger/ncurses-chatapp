#ifndef UI_H
#define UI_H

#include <ncurses.h>

#include "utils.h"
#include "config.h"

void draw_box_around_message_box(int x, int y);
void clear_message_box(int x, int y);
void del_char(int x, int y, char* buf);
void clear_all_messages(int y, int x);
void draw_users(TWOD_CHAR_ARR users); 
TWOD_CHAR_ARR get_user_array(char* users); 
void draw_server_message(int y, struct MsgNode* h); 
void draw_messages(int y, struct MsgNode* h, unsigned int scroll); 
void clear_user_box(); 

#endif // UI_H
