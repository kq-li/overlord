#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "network.h"

socket_node *newSocketNode(int sock, char *description) {
  socket_node *ret = (socket_node *) malloc(sizeof(socket_node));
  ret->sock = sock;
  ret->description = (char *) malloc(strlen(description));
  strcpy(ret->description, description);
  ret->next = NULL;
  return ret;
}

void addSocketToList(socket_list *list, int sock, char *description) {
  list->end->next = newSocketNode(sock, description);
  list->size++;
}

void freeSocketNode(socket_node *node) {
  if (node) {
    if (node->description) {
      free(node->description);
    }

    if (node->next) {
      free(node->next);
    }

    free(node);
  }
}

void removeSocketFromList(socket_list *list, int sock) {
  socket_node *node = list->root;
  socket_node *trail = node;

  while (node->sock != sock) {
    trail = node;
    node = node->next;
  }

  if (node->sock == sock) {
    if (trail != node) {
      trail->next = node->next;
    } else {
      list->root = node->next;
    }
    
    freeSocketNode(node);
    list->size--;
  }
}

int checkError(int descriptor, char *description) {
  if (descriptor == -1) {
    printf("%s error %d: %s\n", description, errno, strerror(errno));
    return -1;
  }

  return 0;
}

int serverSocket(int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (checkError(sock, "[SERVER] Socket")) {
    // error handling
    return -1;
  }

  struct sockaddr_in sock_struct;
  sock_struct.sin_family = AF_INET;
  sock_struct.sin_addr.s_addr = INADDR_ANY;
  sock_struct.sin_port = htons(port);

  int bindStatus = bind(sock, (struct sockaddr *) &sock_struct, sizeof(sock_struct));

  if (checkError(bindStatus, "[SERVER] Bind")) {
    // error handling
    return -1;
  }

  return sock;
}

int serverConnect(int sock) {
  int listenStatus = listen(sock, 1);

  if (checkError(listenStatus, "[SERVER] Listen")) {
    return -1;
  }

  struct sockaddr_in sock_struct;
  socklen_t sock_struct_len;

  int connection = accept(sock, (struct sockaddr *) &sock_struct, &sock_struct_len);
  
  if (checkError(connection, "[SERVER] Accept")) {
    return -1;
  }

  printf("[SERVER] Connected to %s\n", inet_ntoa(sock_struct.sin_addr));

  return connection;
}

int clientConnect(char *address, int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (checkError(sock, "[CLIENT] Socket")) {
    return -1;
  }

  struct sockaddr_in sock_struct;
  sock_struct.sin_family = AF_INET;
  inet_aton(address, &sock_struct.sin_addr);
  sock_struct.sin_port = htons(port);

  printf("[CLIENT] Connecting to %s:%d\n", address, port);

  int connectStatus = connect(sock, (struct sockaddr *) &sock_struct, sizeof(sock_struct));

  if (checkError(connectStatus, "[CLIENT] Connect")) {
    return -1;
  }

  return sock;
}
