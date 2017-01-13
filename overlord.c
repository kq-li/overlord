#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "network.h"

#define MAX_INPUT 1024

char *prompt() {
	printf("Enter command: ");

	char *input = (char *) calloc(MAX_INPUT, sizeof(char));
	fgets(input, MAX_INPUT, stdin);
	input[strcspn(input, "\n")] = 0;
	return input;
}

void subServer(int connection) {

}

int main() {
  int sock = serverSocket(5001);

	int isRunning = 1;

	while (isRunning) {
    int connection = serverConnect(sock);

    int pid = fork();

    if (pid == 0) {
      // child
      close(sock);
      subServer(connection);
      exit(0);
    } else {
      // parent
      close(connection);
    }
    
		char *input = prompt();
		printf("%s\n", input);
		free(input);
	}
	
	return 0;
}
