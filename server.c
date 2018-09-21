#include <sys/socket.h> /* socket definitions */
#include <sys/types.h> /* socket types */
#include <arpa/inet.h> /* inet (3) funtions */
#include "server_helper.h"





int main(int argc, char *argv[]){
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  int server = 0;
  if ((server = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    puts("SOCKET FAILURE");
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  uint16_t port = atoi(argv[1]);
  server_address.sin_port = htons(port);


  if (bind (server, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
    puts("BIND FAILURE");
  }

  if (listen(server, 3) < 0){
    puts("LISTEN ERROR");
  }

  while (1){
    struct sockaddr_in client_address;
    int address_size = sizeof(client_address);
    int client = 0;
    if ((client = accept (server, (struct sockaddr*) &client_address,  &address_size)) < 0) {
      puts("ACCEPT ERROR");
    }
    else{
      uint8_t trans_type = read_trans_type(client);
      printf("Trans type: %d\n", trans_type);

      uint64_t file_size = read_file_size(client);
      printf("File size: %d\n", file_size);

      char* file = read_file(client, file_size);
      printf("File contents: %s\n", file);

      uint64_t name_length = read_file_name_length(client);
      printf("File name length %d\n", name_length);

      char* file_name = read_file_name(client, name_length);
      printf("File name: %s\n", file_name);
    }
    

    
    if ( close (client) < 0 ) {
        puts("CONNECTION CLOSE EROOR");
    }
  }

}



