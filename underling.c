#include "util.h"

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

  }
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
    int isRunning = 1;
    char message[MAX_MESSAGE_LENGTH];
    int sock = -1;

    while (isRunning) {
      if (sock < 0) {
        prompt(&address, "Enter overlord address: ", 1);
        sock = clientConnect(address, PORT);
        //usleep(1000000);

        if (sock >= 0) {
          printf("Connected to overlord.\n");
        }
      } else {
        int length = read(sock, message, MAX_MESSAGE_LENGTH);

        if (length > 0) {
          printf("Command received: %s\n", message);
          char **command = parse(message);

          int stdout = dup(STDOUT_FILENO);
          dup2(sock, STDOUT_FILENO);
          execute(command);
          dup2(stdout, STDOUT_FILENO);
          close(stdout);
        } else if (length < 0) {
          isRunning = 0;
        }
      }
    }
    
    close(sock);
	}

	return 0;
}
