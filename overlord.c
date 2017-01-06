#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT 1024

char *prompt() {
	printf("Enter command: ");

	char *input = (char *) calloc(MAX_INPUT, sizeof(char));
	fgets(input, MAX_INPUT, stdin);
	input[strcspn(input, "\n")] = 0;
	return input;
}

int main() {
	int isRunning = 1;

	while (isRunning) {
		char *input = prompt();
		printf("%s\n", input);
		free(input);
	}
	
	return 0;
}
