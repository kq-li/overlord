#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "overlist.h"

client_list *newClientList() {
  client_list *ret = (client_list *) malloc(sizeof(client_list));
  ret->size = 0;
}

client_node *newClientNode(int sock, char *description, char *prefix) {
  client_node *ret = (client_node *) malloc(sizeof(client_node));
  ret->sock = sock;
  ret->description = (char *) malloc(strlen(description) + 1);
  strcpy(ret->description, description);
  ret->prefix = (char *) malloc(strlen(prefix) + 1);
  strcpy(ret->prefix, prefix);
  return ret;
}

void addClientNodeToList(client_list *list, client_node *node) {
  if (list->size < MAX_CLIENTS) {
    list->list[list->size++] = node;
  }
}

void addClientToList(client_list *list, int sock, char *description, char *prefix) {
  addClientNodeToList(list, newClientNode(sock, description, prefix));
}

void freeClientNode(client_node *node) {
  if (node != NULL) {
    if (node->description) {
      free(node->description);
    }

    if (node->prefix) {
      free(node->prefix);
    }

    close(node->sock);

    free(node);
  }
}

void freeClientList(client_list *list) {
  int i = 0;

  for (i = 0; i < list->size; i++) {
    freeClientNode(list->list[i]);
  }

  list->size = 0;
  free(list);
}

void removeClientFromList(client_list *list, int id) {
  freeClientNode(list->list[id]);

  int i = 0;

  for (i = id + 1; i < list->size; i++) {
    list->list[i - 1] = list->list[i];
  }

  list->list[i] = NULL;
  list->size--;
}

void printClientNode(client_node *node) {
  printf("%d\t\t%s\n", node->sock, node->description);
}

void printClientList(client_list *list) {
  printf("ID\t\tSocket\t\tDescription\n");
  printf("--------------------------------------------------------------------------------\n");
  
  int i = 0;

  for (i = 0; i < list->size; i++) {
    printf("%d\t\t", i);
    printClientNode(list->list[i]);
  }
}
