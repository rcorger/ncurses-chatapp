#ifndef UTILS_H
#define UTILS_H

struct MsgNode {
	char sentby[18];
	char msg[2048];
	struct MsgNode* next;
};

typedef struct {
	char user_arr[50][18];
}  TWOD_CHAR_ARR; 

extern int MAX_MESSAGES;
extern int x_draw, y_draw;
extern int scroll_;
extern int max_scroll;
extern int max_message_length;
extern int msg_counter;

void initialize_VARS(int MM); 
struct MsgNode* initialize_node(char msg[2048], char sentby[18]);
void list_append(struct MsgNode** h, char msg[2048], char sentby[18]);
void check_for_too_many_messages(struct MsgNode** head_a); 
void string_slice(const char* str, char* buf, size_t start, size_t end); 
#endif
