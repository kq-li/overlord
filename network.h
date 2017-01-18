int checkError(int descriptor, char *description);

int serverSocket();

int serverConnect(int sock);

int clientConnect(char *address, int port);
