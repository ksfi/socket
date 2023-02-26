#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

typedef struct Accept {
  int sock;
  struct addrinfo *client;
} Accept;

void display(struct addrinfo* list) {
  struct addrinfo *temp = list;
  while (temp != NULL) {
    printf("%s\n", temp->ai_canonname);
    temp = temp->ai_next;
  }
  free(temp);
}

int serverInit() {
  int sockfd, status;
  struct addrinfo hints, *servinfo, their_addr;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) < 0)
    perror("info");

  /* creation of the socket */
  if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0)
    perror("socket");

  int enable;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
    perror("opt");

  if ((bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)) < 0)
    perror("bind");

  printf("servinfo->ai_addr %d\n", servinfo->ai_addr);
  printf("servinfo->ai_addrlen %d\n", servinfo->ai_addrlen);

  int bl = 100;
  if (listen(sockfd, bl) < 0)
    perror("listen");

  return sockfd;
//   close(sockfd);
}

int serverAccept(Accept *acceptIn) {
  int s;
  socklen_t addr_size = sizeof(acceptIn->client);
  if ((s = accept(acceptIn->sock, acceptIn->client, &addr_size) < 0))
    perror("accept");
  return s;
}

Accept *client(int sockfd) {
  Accept *ret = malloc(sizeof(ret));
  struct addrinfo hints, *servinfo;
  socklen_t len;
  int peer, status;

  ret->client = malloc(sizeof(ret->client));
  len = sizeof(ret->client);
  if ((peer = getpeername(sockfd, ret->client, &len)) < 0)
    perror("peer");

//   memset(&hints, 0, sizeof(hints));
//   hints.ai_family = AF_UNSPEC;
//   hints.ai_socktype = SOCK_STREAM;
//   hints.ai_flags = AI_PASSIVE;
// 
//   if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) < 0)
//     perror("info");

//   if ((ret->sock = socket(ret->client->ai_family, ret->client->ai_socktype, ret->client->ai_protocol)) < 0)
//   if ((ret->sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0)
  if ((ret->sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0)
    perror("socket");

  int co;
  if ((co = connect(ret->sock, servinfo->ai_addr, servinfo->ai_addrlen)) < 0)
    perror("connect");

  return ret;
  free(ret);
}

int main() {
  int s = serverInit();
  printf("s %d\n", s);
  Accept *cl = client(s);
  printf("cl %d\n", cl->sock);
}
