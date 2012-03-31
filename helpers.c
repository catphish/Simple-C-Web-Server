#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// This method can be used to print an error and exit if we detect a problem
void * error(char * msg)
{
  printf("%s\n", msg);
  exit(1);
}

// Read a line from a socket into a buffer, returns the number of bytes processed
int read_to_char(int fh, char * line, char to_char, int limit) {
  // An integer to count the number of bytes processed
  int count=0;
  // An integer to track the number of bytes pushed into the line
  int line_position = 0;
  // Temporary storage for a single char
  char byte;
  
  // Read one byte at a time from the socket
  while((read(fh,&byte,1)) > 0) {
    // Increment the byte counter
    count++;
    if(byte == 13) {
      // why must you waste my bandwidth with carriage returns?
    } else if(byte == to_char) {
      // Have we reached the separator? If so, insert a null terminator and return
      line[line_position] = 0;
      return count;
    } else {
      // Insert a byte into the string
      line[line_position] = byte;
      line_position++;
      // Bail out if we overflow the line buffer
      if(line_position + 1 == limit) {
        line[line_position] = 0;
        return count;
      }
    }
  }
}

// Trim whilespace
char * trimwhitespace(char * str)
{
  char *end;
  // Trim leading space
  while(isspace(*str)) str++;
  
  if(*str == 0)
    return str;
  
  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;
  
  // Insert a null terminator and return
  *(end+1) = 0;
  return str;
}
