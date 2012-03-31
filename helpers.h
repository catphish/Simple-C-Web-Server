void * error(char * msg);
int read_to_char(int fh, char * line, char to_char, int limit);
char * trimwhitespace(char * str);

struct http_header {
  char * key;
  char * value;
};

