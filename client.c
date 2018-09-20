
// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NO_FLAGS 0
#define PORT 8888
#define SERVER_ADDRESS "10.77.99.143"
#define TO_NAME "hello.txt"
#define FILE_PATH "test.txt"
#define TRANS_TYPE 0


uint64_t get_file_size(char*);

uint64_t get_message_size();

char* create_message();

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
        create_message();

    }
    close(socket_fd);
}

char* create_message(){
    uint64_t message_size = get_message_size();
    printf("Message size: %d\n", message_size);
    char* message = malloc(message_size + 1);
    message[message_size] = '\0';
    char* curr_pos = message;

    printf("Message: %s\n\n", message);
    uint16_t path_size = strlen(FILE_PATH);
    printf("Path size befoe htons: %d\n", path_size);
    path_size = htons(path_size);
    memcpy(curr_pos, &path_size, sizeof(path_size));
    curr_pos += sizeof(path_size);
    printf("Path size: %d\n", path_size);
     printf("Message: %s\n\n", message);

    memcpy(curr_pos, FILE_PATH, path_size);
    curr_pos += path_size;
    printf("Message: %s\n\n", message);




    
    
    return message;

}


uint64_t get_file_size(char* file_name){
    // opening the file in read mode 
    FILE* fp = fopen(file_name, "r"); 
  
    // checking if the file exist or not 
    if (fp == NULL) { 
        printf("File Not Found!\n"); 
        return 0; 
    } 
  
    fseek(fp, 0L, SEEK_END); 
  
    // calculating the size of the file 
    long int res = ftell(fp); 
  
    // closing the file 
    fclose(fp); 
  
    return res; 
}


uint64_t get_message_size(){
    return strlen(TO_NAME) + sizeof(uint16_t) 
    + strlen(FILE_PATH) + sizeof(uint16_t)
    + get_file_size(FILE_PATH) + sizeof(uint64_t)
    + sizeof(TRANS_TYPE);
}
