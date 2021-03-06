//
// Created by Brandon on 10/2/2018.
//

#include "server_helper.h"

int create_server(uint16_t port_number){
    int server = 0;
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
         char * error_message = strerror(errno);
         perror(error_message);
         exit(-1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port_number);


    if (bind(server, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        char * error_message = strerror(errno);
        perror(error_message);
        exit(-1);
    }
    return server;
}

void run_server(int server_socket_fd) {
    if (listen(server_socket_fd, 3) < 0) {
        char * error_message = strerror(errno);
        perror(error_message);
        exit(-1);
    }
    struct sockaddr_in client_address;
    int address_size = sizeof(client_address);
    int client = 0;

    while (1) {
        if ((client = accept(server_socket_fd, (struct sockaddr*) &client_address, &address_size)) < 0) {
            char * error_message = strerror(errno);
            perror(error_message);
            exit(-1);
        }
            Message message = read_message(client);
            unsigned char* file_begin = message.file;
            unsigned char* file_end = message.file + message.file_size;
            bool is_good = is_good_format(file_begin, file_end);
            if (is_good){
                write_socket(client, SUCCESS_MESSAGE, sizeof(SUCCESS_MESSAGE));
                transform_and_write(message.trans_type, message.file, message.file_size, message.file_name);
            }
            else{
                write_socket(client, ERROR_MESSAGE, sizeof(ERROR_MESSAGE));
            }
        
        if (close(client) < 0) {
            char * error_message = strerror(errno);
            perror(error_message);
            exit(-1);
        }
    }
}


uint8_t read_trans_type(int socket_fd){
    uint8_t trans_type;
    for (int i = 0; i < sizeof(trans_type); i++){
        int bytes_read = read(socket_fd, &trans_type, 1);
        if (bytes_read < 0){
            if (errno == EINTR){
                continue;
            }
            perror(strerror(errno));
            exit(-1);
        }
    }
    return trans_type;
}

uint64_t read_file_size(int socket_fd){
    uint64_t file_size;
    int max_bytes = sizeof(file_size);
    int bytes_read = 0;
    uint64_t* buffer = &file_size;
    while (bytes_read < max_bytes) {
        int res  = read(socket_fd, buffer + bytes_read, max_bytes - bytes_read);
        bytes_read += res;
    }
    //read(socket_fd, &file_size, sizeof(file_size));
    return ntohl(file_size);
}

unsigned char* read_file(int socket_fd, uint64_t file_size){
    unsigned char* file = malloc(file_size + 1);
    file[file_size] = '\0';
    int bytes_read = 0;
    unsigned char* curr = file;
    while (bytes_read < file_size) {
        int res  = read(socket_fd, curr + bytes_read, file_size - bytes_read);
        bytes_read += res;
    }
    return file;
}

uint16_t read_file_name_length(int socket_fd){
    uint16_t name_length;
    int bytes_read = 0;
    int max_bytes = sizeof(name_length);
    uint16_t* buffer = &name_length;
    while (bytes_read < max_bytes) {
        int res  = read(socket_fd, buffer + bytes_read, max_bytes - bytes_read);
        bytes_read += res;
    }
    return ntohs(name_length);
}

char* read_file_name(int socket_fd, uint16_t name_length){
    char* new_name = malloc(name_length + 1);
    new_name[name_length] = '\0';
    int bytes_read = 0;
   unsigned char* curr = new_name;
    while (bytes_read < name_length) {
        int res  = read(socket_fd, curr + bytes_read, name_length - bytes_read);
        bytes_read += res;
    }
    return new_name;
}

Message read_message(int fd){
    Message message;
    message.trans_type = read_trans_type(fd);
    message.file_size = read_file_size(fd);
    message.file = read_file(fd, message.file_size);
    message.name_length = read_file_name_length(fd);
    message.file_name  = read_file_name(fd, message.name_length);
    return message;
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


bool is_good_format(unsigned char* curr_file_pos, unsigned char* end){
    while (curr_file_pos < end) {
        uint8_t type = *curr_file_pos++;
        if (!is_type(type)) { return false; }
        int16_t bytes_read = (type == FORMAT_ONE_TYPE) ?
                             get_format_one_byte_count(curr_file_pos, end) :
                             get_format_two_byte_count(curr_file_pos, end) ;
        if (bytes_read == NO_BYTES_READ) { return false; }
        curr_file_pos += bytes_read;
    }
    return curr_file_pos == end;
}

/**
 * Determines if the char passed in is a byte
 * denoting the type of the current line.
 * Must be equal to the integer 0 or 1.
 * */
bool is_type(unsigned char type){
    return type == FORMAT_ONE_TYPE || type == FORMAT_TWO_TYPE;
}

/**
 * Calculates the amount of bytes a format one line should have.
 * */
uint16_t get_format_one_byte_count(unsigned char* curr_file_pos, unsigned char* file_end){
    uint8_t amount = *curr_file_pos;
    unsigned char* line_end = curr_file_pos + get_format_one_length(amount);
    return (line_end > file_end) ? NO_BYTES_READ : (line_end - curr_file_pos);
}

/**
 * Determines the length of format 1 line based on
 * the amount of numbers in the line.
 * */
uint16_t get_format_one_length(uint8_t amount){
    return  sizeof(amount) + amount * FORMAT_ONE_NUM_SIZE;
}

/**
 * Returns byte count of format two line by reading
 * the numbers in the line.
 * */
uint16_t get_format_two_byte_count(unsigned char* curr_file_pos, unsigned char* file_end){
    unsigned char* line_pos = curr_file_pos;
    uint32_t amount = get_str_as_int32(line_pos, line_pos + FORMAT_TWO_AMOUNT_SIZE);
    line_pos += FORMAT_TWO_AMOUNT_SIZE;
    for (int i = 0; i < amount; i++){
        uint8_t bytes_read = get_format_two_num_size(line_pos);
        line_pos += bytes_read;
        if (is_end_of_line(*line_pos)) { break; }
        if (!is_end_of_number(*line_pos)) { return NO_BYTES_READ; }
        line_pos++;
    }
    return line_pos - curr_file_pos;
}

uint32_t get_str_as_int32(unsigned char* begin, unsigned char* end){
    uint8_t num_chars = end - begin;
    char* num_as_str = malloc(num_chars + 1);
    num_as_str[num_chars] = '\0';
    memcpy(num_as_str, begin, num_chars);
    uint32_t num =  atoi(num_as_str);
    free(num_as_str);
    return num;
}


uint8_t get_format_two_num_size(unsigned char* curr_file_pos){
    unsigned char* start = curr_file_pos;
    while (!done_parsing_num(start, curr_file_pos)) { curr_file_pos++; }
    return curr_file_pos - start;
}


bool done_parsing_num(unsigned char* start, unsigned char* curr_pos){
    char c = *curr_pos;
    uint64_t bytes_read = curr_pos - start;
    return !isdigit(c) || bytes_read >= FORMAT_TWO_NUM_SIZE;
}


bool is_end_of_line(unsigned char c) {
    return is_type(c) || c == '\n';
}

bool is_end_of_number(unsigned char c) {
    return c == ',';
}

uint16_t to_int16(uint8_t greater_bits, uint8_t lower_bits){
    uint16_t number = greater_bits;
    return (number << 8) | lower_bits;
}

void transform_and_write(uint8_t translation_option, unsigned char* file, uint64_t file_size, unsigned char* file_name){

    FILE* out = fopen(file_name, "w");
    if (!out){
        puts("CANT OPEN FILE");
        return;
    }

    unsigned char* file_end = file + file_size;

    if (translation_option == NO_TRANSLATION) {
        write_no_change(file, file_end, out);
    }

    else if (translation_option == FORMAT_ONE_TO_TWO) {
        write_one_to_two(file, file_end, out);
    }

    else if (translation_option == FORMAT_TWO_TO_ONE){
        write_two_to_one(file, file_end, out);
    }

    else if (translation_option == SWAP_FORMATS){
        write_swapped(file, file_end, out);
    }

    fclose(out);
}

uint16_t read_int16(unsigned char** file) {
    uint16_t higher_bits = *(*file)++;
    uint8_t lower_bits = *(*file)++;
    higher_bits <<= 8;
    uint16_t result = higher_bits | lower_bits;
    return result;
}

uint8_t get_num_digits(uint8_t number) {
    uint8_t digit_count = 0;
    while (number != 0) {
        digit_count++;
        number /= 10;
    }
    return digit_count;
}

unsigned char* to_three_byte_str(uint8_t number) {
    uint8_t num_digits = get_num_digits(number);
    char* str_num = malloc(FORMAT_TWO_AMOUNT_SIZE + 1);
    str_num[FORMAT_TWO_AMOUNT_SIZE] = '\0';
    memset(str_num, '0', FORMAT_TWO_AMOUNT_SIZE);
    int index = FORMAT_TWO_AMOUNT_SIZE - 1;
    for (int i = 0; i < num_digits; i++) {
        str_num[index] = (number % 10) + '0'; // int digit to char
        number /= 10;
    }
    return str_num;
}

void write_one_to_two(unsigned char* curr_pos, unsigned char* file_end, FILE* file) {
    while (curr_pos < file_end) {
        uint8_t type = *curr_pos++;
        if (type == FORMAT_ONE_TYPE) {
            uint8_t amount = *curr_pos++;
            fprintf(file, "%s ", to_three_byte_str(amount));
            for (int i = 0; i < amount - 1; i++) {
                fprintf(file, "%d,", read_int16(&curr_pos));
            }
            fprintf(file, "%d\n", read_int16(&curr_pos));
        }
        else if (type == FORMAT_TWO_TYPE) {
            char amount[FORMAT_TWO_AMOUNT_SIZE + 1];
            amount[FORMAT_TWO_AMOUNT_SIZE] = '\0';
            memcpy(amount, curr_pos, FORMAT_TWO_AMOUNT_SIZE);
            fprintf(file, "%s ", amount);
            curr_pos += FORMAT_TWO_AMOUNT_SIZE;
            while (!is_type(*curr_pos)) {
                fprintf(file, "%c", *curr_pos++);
            }
            fprintf(file, "%c", '\n');
        }
    }
}


void write_two_to_one(unsigned char* curr_pos, unsigned char* file_end, FILE* file) {

    while (curr_pos < file_end) {
        uint8_t type = *curr_pos++;
        if (type == FORMAT_ONE_TYPE) {
            uint8_t amount = *curr_pos++;
            fprintf(file, "%s ", to_three_byte_str(amount));
            for (int i = 0; i < amount - 1; i++) {
                fprintf(file, "%d ", read_int16(&curr_pos));
            }
            fprintf(file, "%d\n", read_int16(&curr_pos));
        }
        else if (type == FORMAT_TWO_TYPE) {
            char amount[FORMAT_TWO_AMOUNT_SIZE + 1];
            amount[FORMAT_TWO_AMOUNT_SIZE] = '\0';
            memcpy(amount, curr_pos, FORMAT_TWO_AMOUNT_SIZE);
            fprintf(file, "%s ", amount);
            curr_pos += FORMAT_TWO_AMOUNT_SIZE;
            while (!is_type(*curr_pos)) {
                char c = *curr_pos++;
                if (c == ',') {
                    fprintf(file, "%c", ' ');
                }
                else {
                    fprintf(file, "%c", c);
                }
            }
            fprintf(file, "%c", '\n');
        }
    }
}

void write_swapped(unsigned char* curr_pos, unsigned char* file_end, FILE* file) {

    while (curr_pos < file_end) {
        uint8_t type = *curr_pos++;
        if (type == FORMAT_ONE_TYPE) {
            uint8_t amount = *curr_pos++;
            fprintf(file, "%s ", to_three_byte_str(amount));
            for (int i = 0; i < amount - 1; i++) {
                fprintf(file, "%d,", read_int16(&curr_pos));
            }
            fprintf(file, "%d\n", read_int16(&curr_pos));
        }
        else if (type == FORMAT_TWO_TYPE) {
            char amount[FORMAT_TWO_AMOUNT_SIZE + 1];
            amount[FORMAT_TWO_AMOUNT_SIZE] = '\0';
            memcpy(amount, curr_pos, FORMAT_TWO_AMOUNT_SIZE);
            fprintf(file, "%s ", amount);
            curr_pos += FORMAT_TWO_AMOUNT_SIZE;
            while (!is_type(*curr_pos)) {
                char c = *curr_pos++;
                if (c == ','){
                    fprintf(file, "%c", ' ');
                }
                else{
                    fprintf(file, "%c", c);
                }
            }
            fprintf(file, "%c", '\n');
        }
    }
}

void write_no_change(unsigned char* curr_pos, unsigned char* file_end, FILE* file){
    while (curr_pos < file_end) {
        uint8_t type = *curr_pos++;
        if (type == FORMAT_ONE_TYPE) {
            uint8_t amount = *curr_pos++;
            fprintf(file, "%s ", to_three_byte_str(amount));
            for (int i = 0; i < amount - 1; i++) {
                fprintf(file, "%d ", read_int16(&curr_pos));
            }
            fprintf(file, "%d\n", read_int16(&curr_pos));
        }
        else if (type == FORMAT_TWO_TYPE) {
            char amount[FORMAT_TWO_AMOUNT_SIZE + 1];
            amount[FORMAT_TWO_AMOUNT_SIZE] = '\0';
            memcpy(amount, curr_pos, FORMAT_TWO_AMOUNT_SIZE);
            fprintf(file, "%s ", amount);
            curr_pos += FORMAT_TWO_AMOUNT_SIZE;
            while (!is_type(*curr_pos)) {
                fprintf(file, "%c", *curr_pos++);
            }
            fprintf(file, "%c", '\n');
        }
    }
}