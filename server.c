#include <sys/socket.h> /* socket definitions */
#include <sys/types.h> /* socket types */
#include <arpa/inet.h> /* inet (3) funtions */
#include <unistd.h> /* misc. UNIX functions */
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

uint16_t read_path_length(int);

char* read_path(int, int);

uint8_t read_trans_type(int);

uint64_t read_file_size(int);

uint8_t read_trans_type(int);

uint64_t read_file_size(int);

char* read_file(int, uint64_t);


int main(int argc, char *argv[]){
  if (argc < 2) {
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }

  int socket_fd = 0;
  if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
    puts("SOCKET FAILURE");
  }

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  uint16_t port = atoi(argv[1]);
  server_address.sin_port = htons(port);


  if (bind (socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
    puts("BIND FAILURE");
  }

  if (listen(socket_fd, 3) < 0){
    puts("LISTEN ERROR");
  }

  while (1){
    struct sockaddr_in client_address;
    int address_size = sizeof(client_address);
    int client_connection = 0;
    if ((client_connection = accept (socket_fd, (struct sockaddr*) &client_address,  &address_size)) < 0) {
      puts("ACCEPT ERROR");
    }
    else{
      uint16_t path_length = read_path_length(client_connection);
      char* path = read_path(client_connection, path_length);
      printf("Path size: %d, Path: %s\n", path_length, path);
      uint8_t trans_type = read_trans_type(client_connection);
      uint64_t file_size = read_file_size(client_connection);
      printf("Trans type: %d\nFile size: %d\n", trans_type, file_size);
      char* file = read_file(client_connection, file_size);
      //puts(file);
      fflush(stdout);
    }
    

    
    if ( close (client_connection) < 0 ) {
        puts("CONNECTION CLOSE EROOR");
    }
  }

}


uint16_t read_path_length(int socket_fd){
  uint16_t path_size;
  read(socket_fd, &path_size, sizeof(path_size));
  return ntohs(path_size);
}

char* read_path(int socket_fd, int path_length){
  char* path = malloc(path_length + 1);
  path[path_length] = '\0';
  read(socket_fd, path, path_length);
  return path;
}

uint8_t read_trans_type(int socket_fd){
  uint8_t trans_type;
  read(socket_fd, &trans_type, sizeof(trans_type));
  return trans_type;
}

uint64_t read_file_size(int socket_fd){
  uint64_t file_size;
  read(socket_fd, &file_size, sizeof(file_size));
  return file_size;

}

char* read_file(int socket_fd, uint64_t file_size){
  char* file = malloc(file_size + 1);
  file[file_size] = '\0';
  read(socket_fd, file, file_size);
  return file;
}