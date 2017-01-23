#define MAX_MESSAGE_LENGTH 1024
#define PORT 5001

int serverSocket();

int serverConnect(int sock);

int clientConnect(char *address, int port);
