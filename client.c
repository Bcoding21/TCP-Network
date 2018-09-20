// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8888
#define SERVER_ADDRESS "173.230.32.253"

char* get_int16_as_str(uint16_t);

int create_client(char*, uint16_t);

int main(int argc, char const *argv[])
{
    int socket_fd = 0;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        puts("SOCKET MAKE ERROR");
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_aton(SERVER_ADDRESS, &server_address.sin_addr) <= 0){
        puts("INET ERROR");
    }

    if ( (connect( socket_fd, (struct sockaddr*) &server_address, sizeof(server_address) )) < 0){
        puts("CONNECT ERROR");
    }
    else{
        puts("CONNECTED");
    }
    close(socket_fd);
}

int create_client(char* ip, uint16_t port_number){
  int sock = 0;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
      printf("\n Socket creation error \n");
      return -1;
  }


  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port_number);

  // Convert IPv4 and IPv6 addresses from text to binary form

  if(inet_aton(ip, &serv_addr.sin_addr)<=0)
  {
      printf("\nInvalid address/ Address not supported \n");
      return -1;
  }
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
      printf("\nConnection Failed \n");
      return -1;
  }
  return sock;
}

char* get_int16_as_str(uint16_t number){
  char* str_num = malloc( sizeof(number) + 1);
  str_num[0] = (unsigned char)(number & 0x00FF);
  str_num[1] = (unsigned char)(number & 0xFF00);
  str_num[2] = '\0';
  return str_num;
}
