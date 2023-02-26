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

#define SA struct addrinfo
#define PORT "3490"
#define PORT2 "80"
#define CO "https"

void bar() {
  printf("---------\n");
}

void display(SA *liste) {
  SA *temp = liste;
  while (temp != NULL) {
    printf("ai_flags %d\n", temp->ai_flags);
    printf("ai_family %d\n", temp->ai_family);
    printf("ai_socktype %d\n", temp->ai_socktype);
    printf("ai_protocol %d\n", temp->ai_protocol);
    printf("ai_addrlen %u\n", temp->ai_addrlen);
    bar();
    temp = temp->ai_next;
  }
}

/* on veut être le client */
int main() {
  int sockfd, sockfd1, co, getaddr, getaddr1, w, opt;
  SA hints, *serv, hints1, *serv1;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

//   memset(&hints1, 0, sizeof(hints1));
//   hints1.ai_family = AF_UNSPEC;
//   hints1.ai_socktype = SOCK_STREAM;
//   hints1.ai_flags = AI_PASSIVE;
//   if ((getaddr1 = getaddrinfo(NULL, PORT, &hints1, &serv1)) < 0)
//     perror("getaddr");
//   printf("SERV 1-----\n");
//   display(serv1);
//   bar();
//   if ((sockfd1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
//     perror("socket");
//   printf("sockfd1 %d\n", sockfd1);

//   if ((getaddr = getaddrinfo("www.twitter.com", PORT, &hints, &serv)) < 0)
  if ((getaddr = getaddrinfo("www.example.com", PORT2, &hints, &serv)) < 0)
    perror("getaddr");

  if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) < 0)
    perror("socket");

  int yes = 1;
  if ((opt = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) < 0)
    perror("option");

  if ((co = connect(sockfd, serv->ai_addr, serv->ai_addrlen)) < 0)
    perror("connect");

//   char *m = "GET / HTTP/1.1\nHOST: www.example.com";
  char *m = "GET / HTTP/1.1\r\nHost: www.cnn.com\r\nConnection: close\r\n\r\n";
  int len = strlen(m), bSent;
  if ((bSent = send(sockfd, m, len, 0)) < 0)
    perror("send");
  printf("bSent %d\n", bSent);

  int f = 0, r, len2 = 1000000;
  void *buf = malloc(len2);
  if ((r = read(sockfd, buf, len2)) < 0)
    perror("read");
  printf("%s", buf);
}
