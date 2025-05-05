#ifndef CONFIG_H
#define CONFIG_H

#define USERNAME				    "Corger"
#define SERVER_NOTIFICATION_FRONT	'>'
#define SERVER_NOTIFICATION_BACK    '<'
#define SERVER_REPETITION			5			// -> if 3: <<< servermsg >>>
#define SERVER_NOTIF_WHITESPACE		40			// set on individual level.	
#define ADDRESS_CONNECTING		    "127.0.0.1" // this is gonna be changed to a command line argument OBVIOUSLY
#define PORT					    1393
#define BORDER_SIZE				    18
#define MSG_LINE_SIZE			    3
#define WAIT_TIME				    10

/* TODO:
	Clean le code up.
	ARG PARSING FOR SERVER ADDR
*/

#endif // CONFIG_H
