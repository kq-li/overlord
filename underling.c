#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define STANDALONE 1

#define TEXT 1
#define SPACES 2
#define QUOTES 3

int startsWith(char *str, char *key) {
	return strncmp(str, key, strlen(key)) == 0;
}

void leftShift(char *str, int offset) {
	char *trail = str;
	str += offset;

	while (*str) {
		*(trail++) = *(str++);
	}

	*trail = 0;
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

void execute(char **command) {
	int pid = fork();

	if (pid == -1) { //error
		printf("Error %d: %s\n", errno, strerror(errno));
	} else if (pid == 0) { //parent

	} else { //child
		if (execvp(command[0], command) == -1) {
			printf("Error %d: %s\n", errno, strerror(errno));
		}
	}
}

int main() {
	if (STANDALONE) { //debugging without overlord
		char input[1024];
		fgets(input, 1024, stdin);
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

	}

	return 0;
}
