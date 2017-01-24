#include "util.h"
#include "overlist.h"

#define MAX_INPUT 1024

#define EXIT 0
#define DEFAULT 1
#define ORDER 2
#define MANAGE 3

#define COMMAND 0
#define CONNECT 1
#define NETWORK 2
#define OTHER 3

char *input;
char message[MAX_MESSAGE_LENGTH];
int sock;
client_list *underlings;
volatile sig_atomic_t logicState;
volatile sig_atomic_t waitState;
sigjmp_buf sigint_jmps[OTHER];

void cleanup() {
  if (input) {
    free(input);
  }

  if (underlings) {
    freeClientList(underlings);
  }

  if (sock >= 0) {
    close(sock);
  }
}

static void sighandler(int signo) {
  if (signo == SIGINT) {
    if (logicState == DEFAULT) {
      cleanup();
      exit(1);
    } else if (waitState == COMMAND) {
      logicState = DEFAULT;
      siglongjmp(sigint_jmps[waitState], 1);
    } else if (waitState == CONNECT) {
      siglongjmp(sigint_jmps[waitState], 1);
    } else if (waitState == NETWORK) {
      printf("hello");
      char data = 3;
      write(sock, &data, 1);
      siglongjmp(sigint_jmps[waitState], 1);
    } else {
      siglongjmp(sigint_jmps[COMMAND], 1);
    }
  }
}

int serverSocket(int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (checkError(sock, "socket") < 0) {
    // error handling
    return -1;
  }

  struct sockaddr_in sock_struct;
  sock_struct.sin_family = AF_INET;
  sock_struct.sin_addr.s_addr = INADDR_ANY;
  sock_struct.sin_port = htons(port);

  int bindStatus = bind(sock, (struct sockaddr *) &sock_struct, sizeof(sock_struct));

  if (checkError(bindStatus, "bind") < 0) {
    // error handling
    return -1;
  }

  return sock;
}

int serverConnect(int sock) {
  int listenStatus = listen(sock, 1);

  if (checkError(listenStatus, "listen") < 0) {
    return -1;
  }

  if (sigsetjmp(sigint_jmps[CONNECT], 1) != 0) {
    return -1;
  }

  struct sockaddr_in sock_struct;
  socklen_t sock_struct_len = sizeof(sock_struct);

  int connection = accept(sock, (struct sockaddr *) &sock_struct, &sock_struct_len);
  
  if (checkError(connection, "accept") < 0) {
    return -1;
  }

  printf("Connected to %s\n", inet_ntoa(sock_struct.sin_addr));

  return connection;
}

int main() {
  input = NULL;
  sock = -1;

  int port = -1;

  while (sock < 0) {
    waitState = OTHER;
    prompt(&input, "Enter port to listen on (default 5001): ", 0);

    if (!input) {
      exit(1);
    }

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

  underlings = newClientList();
  logicState = DEFAULT;
  waitState = COMMAND;
  signal(SIGINT, sighandler);
  
  int index = -1;

  while (sigsetjmp(sigint_jmps[COMMAND], 1) != 0);
  
	while (logicState) {
    switch (logicState) {
    case DEFAULT:
      waitState = COMMAND;
      prompt(&input, "[DEFAULT]> ", 1);
      waitState = OTHER;
      
      if (!input || startsWith(input, "exit")) {
        logicState = EXIT;
      } else if (startsWith(input, "help")) {
      
      } else if (startsWith(input, "order")) {
        logicState = ORDER;
      } else if (startsWith(input, "manage")) {
        logicState = MANAGE;
      } 

      break;

    case ORDER:
      if (!input) {
        logicState = DEFAULT;
        break;
      }
      
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
        
        if (equals(input, "exit")) {
          logicState = DEFAULT;
          index = -1;
        } else {
          strncpy(message, input, MAX_MESSAGE_LENGTH);

          // write command
          write(underling->sock, message, MAX_MESSAGE_LENGTH);

          waitState = NETWORK;

          // read command output
          readMessage(underling->sock);

          // print command output
          printf("%s", message);
        
          // read next prompt
          readMessage(underling->sock);
          strcpy(underling->prefix, message);
        }
      }
      
      break;

    case MANAGE:
      waitState = COMMAND;
      prompt(&input, "[MANAGE]> ", 1);
      waitState = OTHER;

      if (!input || startsWith(input, "back")) {
        logicState = DEFAULT;
      } else if (startsWith(input, "add")) {
        waitState = CONNECT;
        printf("Waiting for underling to connect...\n");

        int connection = serverConnect(sock);

        if (connection >= 0) {
          printf("Underling connected.\n");

          readMessage(connection);
          prompt(&input, "Enter a description for this underling: ", 0);

          addClientToList(underlings, connection, input, message);
        }
      } else if (startsWith(input, "remove")) {
        printClientList(underlings);
        
        int id = -1;

        prompt(&input, "Enter ID to remove: ", 0);

        sscanf(input, "%d", &id);

        if (id < 0 || id > underlings->size) {
          printf("Invalid ID!\n");
        } else {
          removeClientFromList(underlings, id);
        }
      } else if (startsWith(input, "view")) {
        printClientList(underlings);
      }
            
      break;

    default:

      break;
    }
	}
  
	return 0;
}
