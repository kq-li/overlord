#define MAX_CLIENTS 128

typedef struct client_node_struct {
  int pid;
  int sock;
  char *description;
} client_node;

typedef struct client_list_struct {
  client_node *list[128];
  int size;
} client_list;

client_list *newClientList();

client_node *newClientNode(int pid, int sock, char *description);

void addClientNodeToList(client_list *list, client_node *node);

void addClientToList(client_list *list, int pid, int sock, char *description);

void freeClientNode(client_node *node);

void freeClientList(client_list *list);

void removeClientFromList(client_list *list, int pid);

client_node *findClientInList(client_list *list, int pid);

void printClientNode(client_node *node);

void printClientList(client_list *list);
