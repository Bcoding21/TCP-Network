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

int main(int argc, char *argv[])
{
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     int socket_fd = create_server(atoi(argv[1]));
     run_server(socket_fd);
     return 0;
}

struct sockaddr_in create_socket_address(uint16_t port_number){
  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port_number);
  return server_address;
}

int create_server(uint16_t port_number){
  int socket_fd = 0;
  if (socket_fd = socket(AF_INET, SOCK_STREAM, 0) < 0){
    // display error
  }
  struct sockaddr_in server_address = create_socket_address(port_number);
  if (bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
    // display error
  }
  return socket_fd;
}


void run_server(int socket_fd){
  if (listen(socket_fd, 3) < 0){
    // display error
  }
  while(1){
    int incoming_conn = 0;
    if ((incoming_conn = accept(socket_fd, NULL, NULL)) < 0){
      uint16_t number = readBytesAsInt16(socket_fd);
      printf("Here is the message: %d\n",number);
      int bytes_written = write(incoming_conn,"I got your message",18);
      // check if bytes written
      if (close(incoming_conn) < 0){
        // display error
      }
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
