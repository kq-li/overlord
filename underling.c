#include "util.h"

#define LOCALHOST "127.0.0.1"

#define STANDALONE 0

#define TEXT 1
#define SPACES 2
#define QUOTES 3

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

void execute(char **command) {
	int pid = fork();

	if (pid == -1) { //error
		printf("Error %d: %s\n", errno, strerror(errno));
	} else if (pid == 0) { //child
		if (execvp(command[0], command) == -1) {
			printf("Error %d: %s\n", errno, strerror(errno));
		}
	} else { //parent
    waitpid(pid, NULL, 0);
  }
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
	
	char *prefix = (char *) calloc(sizeof(char), strlen(cwd) + strlen(hostname) + 64);
	sprintf(prefix,
					//BOLD_RED
          "%s:"
          //BOLD_BLUE
          "%s $ "
          //BOLD_GREEN
          ,
          hostname, cwd);

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
    char *input = NULL;
    char *address = NULL;
    int port = -1;
    int isRunning = 1;
    char message[MAX_MESSAGE_LENGTH];
    int sock = -1;
    char *prefix = NULL;
    char **command = NULL;

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
            printf("Connected to overlord.\n");
            write(sock, getPrompt(), MAX_MESSAGE_LENGTH);
          }
        }
      } else {
        prefix = getPrompt();
        printf("%s", prefix);
        
        int length = read(sock, message, MAX_MESSAGE_LENGTH);

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

          execute(command);

          dup2(stdout_copy, STDOUT_FILENO);
          dup2(stderr_copy, STDERR_FILENO);

          close(stdout_copy);
          close(stderr_copy);

          read(pd[0], message, MAX_MESSAGE_LENGTH);
          close(pd[0]);

          printf("%s", message);
          write(sock, message, MAX_MESSAGE_LENGTH);
          write(sock, prefix, MAX_MESSAGE_LENGTH);
        } else if (length <= 0) {
          printf("Lost connection to overlord. Exiting...\n");
          isRunning = 0;
        }
      }
    }
    
    close(sock);

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
