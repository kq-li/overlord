#include "util.h"

#define LOCALHOST "127.0.0.1"

#define STANDALONE 0

#define TEXT 1
#define SPACES 2
#define QUOTES 3

char *input;
char message[MAX_MESSAGE_LENGTH];
int sock;
sigset_t sigset;
struct pollfd pfd[2];

int clientConnect(char *address, int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (checkError(sock, "socket") < 0) {
    return -1;
  }

  struct sockaddr_in sock_struct;
  sock_struct.sin_family = AF_INET;
  inet_aton(address, &sock_struct.sin_addr);
  sock_struct.sin_port = htons(port);

  printf("Connecting to %s:%d\n", address, port);

  int connectStatus = connect(sock, (struct sockaddr *) &sock_struct, sizeof(sock_struct));

  if (checkError(connectStatus, "connect") < 0) {
    return -1;
  }

  return sock;
}

// This time, I'm hardcoding the escape sequences.
char **parse(char *str) {
  char **ret = (char **) calloc(sizeof(char *), strlen(str));
	char *trail = str;
	int state = TEXT;
	int i = 0;

	while (*str) {
		switch (state) {
		case TEXT: // 1
			if (*str == '\\') {
				leftShift(str, 1);
				str++;
			} else if (*str == '"') {
				state = QUOTES;
				leftShift(str, 1);
			} else if (*str == ' ') {
				state = SPACES;
				*str = 0;
				ret[i++] = trail;
				str++;
			} else {
				str++;
			}			

			break;

		case SPACES: // 2
			if (*str == ' ') {
				leftShift(str, 1);
			} else {
				state = TEXT;
				trail = str;
			}
			
			break;

		case QUOTES: // 3
			if (*str == '\\') {
				leftShift(str, 1);
				str++;
			} else if (*str == '"') {
				state = TEXT;
				leftShift(str, 1);
			} else {
				str++;
			}
			
			break;
		}
	}

	ret[i] = trail;
	return ret;
}

int execute(char **command) {
  if (equals(command[0], "cd")) {
		if (command[1]) {
			if (*command[1] == '~') {
				chdir(getenv("HOME"));

				if (*(command[1] + 1)) {
					leftShift(command[1], 2);

					if (chdir(command[1]) != 0) {
						switch (errno) {
						case ENOTDIR:
							printf("%s: %s: Not a directory\n", command[0], command[1]);
							break;
					
						case ENOENT:
							printf("%s: %s: No such file or directory\n", command[0], command[1]);
							break;

						default:
							printf("Error %d: %s\n", errno, strerror(errno));
							break;
						}
					}
				}
			} else if (chdir(command[1]) != 0) {
				switch (errno) {
				case ENOTDIR:
					printf("%s: %s: Not a directory\n", command[0], command[1]);
					break;
					
				case ENOENT:
					printf("%s: %s: No such file or directory\n", command[0], command[1]);
					break;

				default:
					printf("Error %d: %s\n", errno, strerror(errno));
					break;
				}
			}
		} else {
			chdir(getenv("HOME"));
		}
  } else {
    int pid = fork();

    if (pid == -1) { //error
      printf("Error %d: %s\n", errno, strerror(errno));
    } else if (pid == 0) { //child
      if (execvp(command[0], command) == -1) {
        printf("Error %d: %s\n", errno, strerror(errno));
      }
    } else { //parent
      int pollStatus = poll(pfd, 2, -1);

      if (checkError(pollStatus, "ppoll") > 0) {
        if (pfd[0].revents) { // SIGCHLD

        } else { // socket
          if (pfd[1].revents & POLLIN) {
            char data;
            read(sock, &data, MAX_MESSAGE_LENGTH);
            kill(pid, SIGINT);
            return -1;
          }
        }
      }
    }
  }

  return 0;
}

char *getPrompt() {
  char *cwd = getcwd(NULL, 0);
	char *home = getenv("HOME");
	
	if (startsWith(cwd, home)) {
		leftShift(cwd, strlen(home) - 1);
		*cwd = '~';
	}
	
	char hostname[256];
	gethostname(hostname, 256);
	
	char *prefix = (char *) calloc(sizeof(char), strlen(cwd) + strlen(hostname) + 5);
	sprintf(prefix, "%s:%s $ ", hostname, cwd);
  free(cwd);

  return prefix;
}

int main() {
	if (STANDALONE) { //debugging without overlord
		char input[MAX_MESSAGE_LENGTH];
		fgets(input, MAX_MESSAGE_LENGTH, stdin);
		input[strcspn(input, "\n")] = 0;

		char **command = parse(input);
		char **sp = command;

		printf("%s", *(sp++));
		
		while (*sp) {
			printf(".%s", *sp);
			sp++;
		}

		printf("\n");
		
	} else {
    char *address = NULL;
    int port = -1;
    int isRunning = 1;
    input = NULL;
    sock = -1;
    char *prefix = NULL;
    char **command = NULL;

    sigset_t sigset;
    int sigchld_fd;
    
    if (checkError(sigemptyset(&sigset), "sigemptyset") < 0 ||
        checkError(sigaddset(&sigset, SIGCHLD), "sigaddset") < 0 ||
        checkError(sigprocmask(SIG_BLOCK, &sigset, NULL), "sigprocmask") < 0 ||
        (sigchld_fd = checkError(signalfd(-1, &sigset, 0), "signalfd")) < 0) {
      exit(1);
    }

    pfd[0].fd = sigchld_fd;
    pfd[0].events = POLLIN | POLLERR | POLLHUP;
    
    while (isRunning) {
      if (sock < 0) {
        prompt(&input, "Enter overlord address (default 127.0.0.1): ", 1);

        if (*input) {
          address = (char *) malloc(strlen(input) + 1);
          strcpy(address, input);
        } else {
          address = (char *) malloc(strlen(LOCALHOST) + 1);
          strcpy(address, LOCALHOST);
        }
        
        prompt(&input, "Enter port (default 5001): ", 1);

        if (*input) {
          sscanf(input, "%d", &port);
        } else {
          port = PORT;
        }
        
        printf("%s:%d\n", address, port);
        
        if (address == NULL || port == -1) {
          printf("Invalid input!\n");
        } else {
          sock = clientConnect(address, port);
          //usleep(1000000);

          if (sock >= 0) {
            pfd[1].fd = sock;
            pfd[1].events = POLLIN | POLLERR | POLLHUP;
            
            printf("Connected to overlord.\n");
            prefix = getPrompt();
            write(sock, prefix, MAX_MESSAGE_LENGTH);
          }
        }
      } else {
        printf("[UNDERLING] %s", prefix);
        fflush(stdout);
        
        int length = readMessage(sock);

        if (length > 0) {
          printf("%s\n", message);
          command = parse(message);
          
          int pd[2];
          pipe(pd);
          
          int stdout_copy = dup(STDOUT_FILENO);
          int stderr_copy = dup(STDERR_FILENO);
          dup2(pd[1], STDOUT_FILENO);
          dup2(pd[1], STDERR_FILENO);
          close(pd[1]);

          int executeStatus = execute(command);

          dup2(stdout_copy, STDOUT_FILENO);
          dup2(stderr_copy, STDERR_FILENO);

          close(stdout_copy);
          close(stderr_copy);

          if (executeStatus >= 0) {
            readMessage(pd[0]);

            printf("%s", message);
            write(sock, message, MAX_MESSAGE_LENGTH);

            free(prefix);
            prefix = getPrompt();
            write(sock, prefix, MAX_MESSAGE_LENGTH);
          }

          close(pd[0]);
        } else if (length <= 0) {
          printf("exit\n");
          isRunning = 0;
        }
      }
    }

    if (sock >= 0) {
      close(sock);
    }

    if (input) {
      free(input);
    }

    if (address) {
      free(address);
    }

    if (command) {
      free(command);
    }

    if (prefix) {
      free(prefix);
    }
	}

	return 0;
}
