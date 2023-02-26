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

#define PORT "3490"
#define SA struct addrinfo

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sigchld_handler(int s) {
  int saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}

void zomb() {
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) < 0)
    perror("sigaction");
}

int acceptLoop(int sockfd) {
  int newfd, s;
  struct sockaddr_storage their_addr;
  while (1) {
    printf("ici\n");
    socklen_t sin_size = sizeof(their_addr);
    if ((newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size) < 0))
      perror("newfd");
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
    printf("server: got connection from %s\n", s);
    if (!fork()) {
      close(sockfd);
      if (send(newfd, "hello world", 11, 0) < 0)
        perror("send");
      close(newfd);
      exit(0);
    }
    close(newfd);
  }
  return 0;
}

/* create a socket */
int createS() {
  int sockfd, status, opt, yes;
  SA hints, *servinfo, *p;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) < 0)
    perror("status");

  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0) { 
      perror("socket");
      continue;
    }
    if ((opt = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))) < 0) {
      perror("opt");
      exit(1);
    }

    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
      perror("bind");
      continue;
    }
    break;
  }

  freeaddrinfo(servinfo);
  if (p == NULL) fprintf(stderr, "server didn't blind\n");

  if (listen(sockfd, 10) < 0)
    perror("listen");

  zomb();

  acceptLoop(sockfd);

  return sockfd;
}

int main() {
  int i = createS();
  printf("i %d\n", i);
}
