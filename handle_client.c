#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>

#include "helpers.h"
#include "config.h"

// Handle an incoming socket, accepts a pointer to a client file descriptor
void * handle_client(void * p_clientfd) {
  // Allocate pointers for up to 50 headers
  struct http_header headers[MAX_HEADERS];
  
  // A counter for the actual number of headers
  int header_count=0;
  
  // Get the file descriptor of the socket into a local variable
  int clientfd = *(int*)p_clientfd;
  
  // Free the temporary descriptor that was passed to us from the parent process
  free((int*)p_clientfd);
  
  // Storage for header lines
  char line[MAX_HEADER_LINE];
  
  // Storage for the request line
  char request[MAX_REQUEST];
  
  // Temporary variables for use whenever
  char * temp; int n;
  // A temporary variable for use with strtok_r
  char * saveptr;
  
  // Read one line (the request)
  read_to_char(clientfd, request, NEWLINE, MAX_HEADER_LINE);
  
  // Read each line of headers
  while(read_to_char(clientfd, line, NEWLINE, MAX_HEADER_LINE) > 0 && strlen(line) > 0) {
    // Split off characters up to the first colon (the header key)
    temp = strtok_r(line, ":", &saveptr);
    // Allocate memory for the header key
    headers[header_count].key = malloc(strlen(temp));
    // Copy the header key to the struct in the array
    strcpy(headers[header_count].key, temp);
    
    // Trim the remainder of the line (the header value)
    temp = trimwhitespace(saveptr);
    // Alocate memory for the value
    headers[header_count].value = malloc(strlen(temp));
    // Copy the header value to the struct in the array
    strcpy(headers[header_count].value, temp);
    // Increment the number of headers
    header_count++;
    
    // Bail out if we overflow the number of headers
    if(header_count == MAX_HEADERS) break;
  }
  
  /*
    Real routing would go here
  */
  
  // Print the HTTP status (to a string then to the socket)
  sprintf(line, "HTTP/1.1 200 OK\r\n\r\n");
  write(clientfd, line, strlen(line));
  
  // Print the body of the page
  sprintf(line, "I'm a mostly useless web server written in C!\r\n\r\n");
  write(clientfd, line, strlen(line));
  sprintf(line, "Thanks for the request, it looked like this:\r\n\r\n");
  write(clientfd, line, strlen(line));
  sprintf(line, "%s\r\n", request);
  write(clientfd, line, strlen(line));
  
  // Loop through the headers and echo them back to the user
  for(n=0; n < header_count; n++) {
    sprintf(line, "%s=%s\n", headers[n].key, headers[n].value);
    write(clientfd, line, strlen(line));
  }
  
  // Close the socket
  close(clientfd);
  // Free memory allocated for headers
  for(n=0; n < header_count; n++) {
    free(headers[n].key);
    free(headers[n].value);
  }
  
  // We can't return anything useful as we're a thread
  return;
}
