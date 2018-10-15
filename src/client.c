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
#include <inttypes.h>
#include <errno.h>

#define NO_FLAGS 0
#define SERVER_IP 1
#define SERVER_PORT 2
#define FILE_PATH 3
#define TO_FORMAT 4
#define TO_NAME 5
#define RESPONSE_SIZE 50
#define NUM_ARGS_NEEDED 6

int write_socket(int, void*, int);

extern int errno;

unsigned char* read_response(int, int);

extern int errno;

uint64_t get_file_size(const char*);

uint64_t get_message_size(const char*, const char*, uint8_t);

unsigned char* create_message(const char*, const char*, uint8_t);

bool does_exist(const char*);

bool is_empty(const char*);

int init_socket(const char *, const char *);

void write_bytes(int, unsigned char*, uint64_t);

unsigned char* read_bytes(int, uint64_t);


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

  int socket = init_socket(argv[SERVER_IP], argv[SERVER_PORT]);
  
  uint64_t message_size = get_message_size(file_path, argv[TO_NAME], to_format);
  unsigned char* message = create_message(file_path, argv[TO_NAME], to_format);
  
  write_socket(socket, message, message_size);
  char* response = read_response(socket, RESPONSE_SIZE);
  puts(response);
  free(response);
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

int init_socket(const char *server_ip, const char *server_port){
    int socket_fd = 0;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        char * error_message = strerror(errno);
        perror(error_message);
        exit(-1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(server_port));

    if (inet_aton(server_ip, &server_address.sin_addr) <= 0){
        char * error_message = strerror(errno);
        perror(error_message);
        exit(-1);
    }

    if ( (connect( socket_fd, (struct sockaddr*) &server_address, sizeof(server_address) )) < 0){
        char * error_message = strerror(errno);
        perror(error_message);
        exit(-1);
    }
    return socket_fd;
}

uint64_t get_file_size(const char* file_name){
    struct stat st;
    stat(file_name, &st);
    return st.st_size;
}

unsigned char* create_message(const char* file_path, const char* to_name, uint8_t to_format){

    uint64_t message_size = get_message_size(file_path, to_name, to_format);
    unsigned char* message = malloc(message_size);
    unsigned char* curr_pos = message; // used to keep track of where we are in message array as we add data

    memcpy(curr_pos++, &to_format, sizeof(to_format));

    uint64_t file_size = get_file_size(file_path);
    file_size = htonl(file_size); // set endian
    memcpy(curr_pos, &file_size, sizeof(file_size));
    file_size = ntohl(file_size); // set back to original endian
    curr_pos += sizeof(file_size);

    FILE* file = fopen(file_path, "rb");
    curr_pos += fread(curr_pos, sizeof(char), file_size, file);
    fclose(file);

    uint16_t to_name_size = strlen(to_name);
    to_name_size = htons(to_name_size);
    memcpy(curr_pos, &to_name_size, sizeof(to_name_size));
    to_name_size = ntohs(to_name_size);
    curr_pos += sizeof(to_name_size);

    memcpy(curr_pos, to_name, to_name_size);
    return message;
}

uint64_t get_message_size(const char* file_path, const char* new_name, uint8_t to_format){
    uint64_t file_size = get_file_size(file_path);
    uint16_t file_name_size = strlen(new_name);
    return sizeof(to_format) + sizeof(file_name_size) + file_name_size
    + sizeof(file_size) + file_size;
}

int write_socket(int fd, void* data , int data_size) {
    while (data_size != 0){
        int bytes_written = write(fd, data, data_size);
        if (bytes_written < 0){
            if (errno == EINTR){
                bytes_written  = 0;
            }
            perror(strerror(errno));
            exit(-1);
        }
        data_size -= bytes_written;
        data += bytes_written;
    }
}

unsigned char* read_response(int fd, int size){
    unsigned char* res = malloc(size + 1);
    res[size] = '\0';
    int bytes_read = 0;
    unsigned char* curr = res;
    int r = 1;
    while (r  > 0) {
        r  = read(fd, curr + bytes_read, size - bytes_read);
        bytes_read += r;
    }
    return res;
}