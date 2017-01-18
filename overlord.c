#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "network.h"
#include "overlist.h"

#define MAX_INPUT 1024

#define EXIT 0
#define DEFAULT 1
#define ORDER 2
#define SPY 3
#define MANAGE 4

char *prompt(char *prefix, int history) {
  //printf("%s", prefix);
	//char *input = (char *) calloc(MAX_INPUT, sizeof(char));
	//fgets(input, MAX_INPUT, stdin);
	//input[strcspn(input, "\n")] = 0;

  char *input = readline(prefix);

  if (history && input && *input) {
    add_history(input);
  }
  
	return input;
}

int startsWith(char *s1, char *s2) {
  return strncmp(s1, s2, strlen(s2)) == 0;
}

int main() {
  printf("Welcome, overlord.\n");
  printf("Type help for available commands.\n");

  int state = DEFAULT;
  char *input;
  client_list *underlings = newClientList();
  int sock = serverSocket(5001);

	while (state) {
    switch (state) {
    case DEFAULT:
      input = prompt("[DEFAULT]> ", 1);

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

      break;

    case SPY:

      break;

    case MANAGE:
      input = prompt("[MANAGE]> ", 1);

      if (startsWith(input, "add")) {
        printf("Waiting for underling to connect...\n");

        int connection = serverConnect(sock);

        printf("Underling connected.\n");

        input = prompt("Enter a description for this underling: ", 0);

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

  close(sock);
  
	return 0;
}
