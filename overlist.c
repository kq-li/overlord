#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "overlist.h"

client_list *newClientList() {
  client_list *ret = (client_list *) malloc(sizeof(client_list));
  ret->size = 0;
}

client_node *newClientNode(int pid, int sock, char *description) {
  client_node *ret = (client_node *) malloc(sizeof(client_node));
  ret->pid = pid;
  ret->sock = sock;
  ret->description = (char *) malloc(strlen(description) + 1);
  strcpy(ret->description, description);
  return ret;
}

void addClientNodeToList(client_list *list, client_node *node) {
  if (list->size < MAX_CLIENTS) {
    list->list[list->size++] = node;
  }
}

void addClientToList(client_list *list, int pid, int sock, char *description) {
  addClientNodeToList(list, newClientNode(pid, sock, description));
}

void freeClientNode(client_node *node) {
  if (node != NULL) {
    if (node->description) {
      free(node->description);
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

void removeClientFromList(client_list *list, int pid) {
  int i = 0;
  int found = 0;

  for (i = 0; i < list->size; i++) {
    if (!found) {
      if (list->list[i] != NULL && list->list[i]->pid == pid) {
        freeClientNode(list->list[i]);
        found = 1;
      }
    } else {
      list->list[i - 1] = list->list[i];
    }
  }

  list->list[i] = NULL;
  list->size--;
}

client_node *findClientInList(client_list *list, int pid) {
  int i = 0;

  for (i = 0; i < list->size; i++) {
    if (list->list[i] != NULL && list->list[i]->pid == pid) {
      return list->list[i];
    }
  }

  return NULL;
}

void printClientNode(client_node *node) {
  printf("%d\t\t%d\t\t%s\n", node->pid, node->sock, node->description);
}

void printClientList(client_list *list) {
  printf("ID\t\tPID\t\tSocket\t\tDescription\n");
  printf("--------------------------------------------------------------------------------\n");
  
  int i = 0;

  for (i = 0; i < list->size; i++) {
    printf("%d\t\t", i);
    printClientNode(list->list[i]);
  }
}
