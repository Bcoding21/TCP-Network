
// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // for close
#include <arpa/inet.h>
#include <stdbool.h>

#define NO_FLAGS 0
#define SERVER_IP 1
#define SERVER_PORT 2
#define FILE_PATH 3
#define TO_FORMAT 4
#define TO_NAME 5
#define RESPONSE_SIZE 50
#define NUM_ARGS_NEEDED 6


uint64_t get_file_size(const char*);

uint64_t get_message_size(uint64_t, const char*, uint8_t);

char* create_message(const char*, uint64_t, const char*, uint8_t);

char* get_file(const char*);

bool does_exist(const char*);

bool is_empty(const char*);

int new_client(const char*, const char*);

char* send_request(int, uint8_t, const char*, const char*);

int main(int argc, char const *argv[]) {
  if (argc < NUM_ARGS_NEEDED) {
    fprintf(stderr, "ERROR, 5 args needed\n");
    exit(1);
  }

  uint8_t to_format = atoi(argv[TO_FORMAT]);
  if (to_format < 0 || to_format > 3) {
    fprintf(stderr, "ERROR, TO FORMAT OUT OF RANGE\n");
    exit(1);
  }
  const char* file_path = argv[FILE_PATH];
  if (!does_exist(file_path)) {
      fprintf(stderr, "ERROR, FILE DOES NOT EXIST\n");
      exit(1);
  }
  if (is_empty(file_path)){
      fprintf(stderr, "ERROR, FILE IS EMPTY\n");
      exit(1);
  }

  int socket_fd = new_client(argv[SERVER_IP], argv[SERVER_PORT]);
  char* response = send_request(socket_fd, to_format, file_path, argv[TO_NAME]);
  printf("Response: %s\n", response);
  free(response);
}


char* send_request(int socket_fd, uint8_t to_format, const char* file_path, const char* to_name){
  uint64_t file_size = get_file_size(file_path);
  char* file = get_file(file_path);
  uint64_t message_size = get_message_size(file_size, to_name, to_format);
  char* message = create_message(file, file_size, to_name, to_format);
  send(socket_fd, message, message_size, NO_FLAGS);

  unsigned char* response_message = malloc(RESPONSE_SIZE + 1);
  response_message[RESPONSE_SIZE] = '\0';
  recv(socket_fd, response_message, RESPONSE_SIZE, NO_FLAGS);
  close(socket_fd);
  free(file);
  free(message);
  return response_message;
}

int new_client(const char* server_ip, const char* server_port){
     int socket_fd = 0;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        puts("SOCKET MAKE ERROR");
        exit(1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(server_port));

    if (inet_aton(server_ip, &server_address.sin_addr) <= 0){
        puts("INET ERROR");
        exit(1);
    }

    if ( (connect( socket_fd, (struct sockaddr*) &server_address, sizeof(server_address) )) < 0){
        puts("CONNECT ERROR");
        exit(1);
    }
    return socket_fd;
}

bool does_exist(const char* file_path){
    FILE* file = fopen(file_path, "r");
    bool does_exist = file != NULL;
    fclose(file);
    return does_exist;
}

bool is_empty(const char* file_path){
    return get_file_size(file_path) == 0;
}
char* create_message(const char* file, uint64_t file_size, const char* to_name, uint8_t to_format){
    // create array to be sent to the server
    uint64_t message_size = get_message_size(file_size, to_name, to_format);

    char* message = malloc(message_size + 1);
    message[message_size] = '\0';
    char* curr_pos = message; // keep track of where we are in the array
                              // because we will increment as we add data

    memcpy(curr_pos, &to_format, sizeof(to_format)); // params (destination, source, num bytes to read)
    curr_pos += sizeof(to_format);

    file_size = htonl(file_size); // reorder arrangement of bytes
    memcpy(curr_pos, &file_size, sizeof(file_size));
    file_size = ntohl(file_size); // put it back
    curr_pos += sizeof(file_size);

    memcpy(curr_pos, file, file_size);
    curr_pos += file_size;

    uint16_t to_name_size = strlen(to_name);
    to_name_size = htons(to_name_size);
    memcpy(curr_pos, &to_name_size, sizeof(to_name_size));
    to_name_size = ntohs(to_name_size);
    curr_pos += sizeof(to_name_size);

    memcpy(curr_pos, to_name, to_name_size);
    curr_pos += to_name_size;

    return message;
}

uint64_t get_message_size(uint64_t file_size, const char* new_name, uint8_t to_format){
    return strlen(new_name) + sizeof(uint16_t)
    + file_size + sizeof(file_size)
    + sizeof(to_format);
}

char* get_file(const char* file_name){
    uint64_t file_size = get_file_size(file_name);
    char* buffer = malloc(file_size + 1);
    buffer[file_size] = '\0';
    FILE* file = fopen(file_name, "r");
    if (file == NULL){  return NULL; }
    fread(buffer, sizeof(*file_name), file_size, file);
    fclose(file);
    return buffer;
}

uint64_t get_file_size(const char* file_name){
    struct stat st;
    stat(file_name, &st);
    return st.st_size;
}
