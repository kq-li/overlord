typedef struct socket_node_struct {
  int sock;
  char *description;
  struct socket_node_struct *next;
} socket_node;

typedef struct socket_list_struct {
  socket_node *root;
  socket_node *end;
  int size;
} socket_list;

socket_node *newSocketNode(int sock, char *description);

void addSocketToList(socket_list *list, int sock, char *description);

void freeSocketNode(socket_node *node);

void removeSocketFromList(socket_list *list, int sock);

int checkError(int descriptor, char *description);

int serverSocket();

int serverConnect(int sock);

int clientConnect(char *address, int port);
