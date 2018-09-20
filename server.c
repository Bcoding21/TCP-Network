#include <sys/socket.h> /* socket definitions */
#include <sys/types.h> /* socket types */
#include <arpa/inet.h> /* inet (3) funtions */
#include <unistd.h> /* misc. UNIX functions */
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

uint16_t readBytesAsInt16(int);

int create_server(uint16_t);

struct sockaddr_in create_socket_address(uint16_t);

void run_server(int);

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
    puts("CONNECTION ACCEPTED");
    if ( close (client_connection) < 0 ) {
        puts("CONNECTION CLOSE EROOR");
    }
  }




}

uint16_t readBytesAsInt16(int socket_id) {
  unsigned char num[2];
  read(socket_id,num,2);
	uint16_t greaterBits = num[1];
	uint8_t lowerBits = num[0];
	return (greaterBits << 8) | lowerBits;
}
