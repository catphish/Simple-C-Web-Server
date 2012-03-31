#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "handle_client.h"
#include "helpers.h"
#include "config.h"

int main(int argc, char *argv[]) {
  // Keep track of a single thread
  pthread_t thread;
  
  // File descriptor for socket
  int serverfd;
  
  // Pointer to a file descriptor to send to new threads
  int * p_clientfd;
  
  // Address of client and server
  struct sockaddr_in serv_addr, cli_addr;
  int clilen = sizeof(cli_addr);
  
  // Set up the address to listen on
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(LISTEN_PORT);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  
  // Create a socket
  serverfd = socket(AF_INET, SOCK_STREAM, 0);
  
  // Allow a recently closed port to be reused
  int so_reuseaddr = 1;
  setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));
  
  // Bind the socket to a local address
  if(bind(serverfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  
  // Listen for new connections
  listen(serverfd, CONNECTION_BACKLOG);
  
  // Loop
  while(1) {
    // Allocate some memory to temporarily store a file descriptor to pass to a new thread
    p_clientfd = (int*)malloc(sizeof(int));
    // Accept a connection, store its descriptor
    *p_clientfd = accept(serverfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
    // Create a thread, pass it the file descriptor of the client
    pthread_create(&thread , NULL, &handle_client, p_clientfd);
    // Forget about the thread and carry on
    pthread_detach(thread);
  }
  
  // Shut down the server - this won't happen given the loop above
  close(serverfd);
  return 0;
}
