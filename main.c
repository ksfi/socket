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

void display(struct addrinfo* list) {
  struct addrinfo *temp = list;
  while (temp != NULL) {
    printf("%s\n", temp->ai_canonname);
    temp = temp->ai_next;
  }
  free(temp);
}

int main() {
  int status, status2, k = 0, s;
  struct addrinfo hints, *servinfo;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

//   if ((status = getaddrinfo("www.geeksforgeeks.org", "http", &hints, &servinfo)) != 0) perror("info");
  if ((status = getaddrinfo("www.google.com", "http", &hints, &servinfo)) != 0) perror("info");
//   if ((status = getaddrinfo("www.example.com", "http", &hints, &servinfo)) != 0) perror("info");

  /* creation of the socket */
  if ((s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
    perror("socket");

  /* connect to the server as a client */
  if (connect(s, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
    perror("connect");

  /* play with requests */
  char msg[] = "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n";
  int i;
  if ((i = send(s, msg, sizeof(msg), 0) < 0))
    perror("send");
  printf("len %lu\n", sizeof(msg));

  int len = 10000000, f = 0, r;
  void *buf = malloc(len);
  if ((r = read(s, buf, len)) < 0)
    perror("read");
  printf("%s\n", buf);

  freeaddrinfo(servinfo);
}
