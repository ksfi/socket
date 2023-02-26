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
#include <signal.h>
#include <errno.h>

#define SA struct addrinfo
#define PORT "3490"

int main(int argc, char *argv[]) {
  int sock, stat, opt;
  SA hints, *servinfo, *temp;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (argc > 1) {
    if ((stat = getaddrinfo(argv[1], PORT, &hints, &servinfo)) < 0)
      perror("stat");
  } else {
    if ((stat = getaddrinfo("www.example.com", PORT, &hints, &servinfo)) < 0)
      perror("stat");
  }

  temp = servinfo;
  while (temp != NULL) {
    if ((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0)
      perror("socket");

    int y = 1;
    if ((opt = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y))) < 0)
      perror("opt");

    temp = temp->ai_next;
  }
}
