#define MAX_MESSAGE_LENGTH 1024
#define PORT 5004

int checkError(int descriptor, char *description);

int serverSocket();

int serverConnect(int sock);

int clientConnect(char *address, int port);
