
#include "server_helper.h"


uint8_t read_trans_type(int socket_fd){
  uint8_t trans_type;
  read(socket_fd, &trans_type, sizeof(trans_type));
  return trans_type;
}

uint64_t read_file_size(int socket_fd){
  uint64_t file_size;
  read(socket_fd, &file_size, sizeof(file_size));
  return ntohl(file_size);
}

unsigned char* read_file(int socket_fd, uint64_t file_size){
  unsigned char* file = malloc(file_size + 1);
  file[file_size] = '\0';
  read(socket_fd, file, file_size);
  return file;
}

uint16_t read_file_name_length(int socket_fd){
  uint16_t name_length;
  read(socket_fd, &name_length, sizeof(name_length));
  return ntohs(name_length);
}

char* read_file_name(int socket_fd, uint16_t name_length){
  char* new_name = malloc(name_length + 1);
  new_name[name_length] = '\0';
  read(socket_fd, new_name, name_length);
  return new_name;
}

bool is_good_format(unsigned char* file, uint64_t file_size){
  uint64_t i = 0;
  while (i < file_size){
    uint8_t type = file[i++];
    if (!is_type(type)) { return false; }
    uint16_t bytes_read = (type) ? 
    read_format_one(file + i, file_size) :
    read_format_two(file + i, file_size);
    if (bytes_read == NO_BYTES_READ) { return false; }
    i += bytes_read;
  }
}

bool is_type(uint8_t type){
  return type == FORMAT_ONE_TYPE || type == FORMAT_TWO_TYPE;
}


uint16_t read_format_one(unsigned char* file, uint64_t file_size){
  uint64_t start = (uint64_t)file;
  if (*(file++) != ' ') { return NO_BYTES_READ;}
  uint8_t amount = *(file++); 
  uint16_t line_size = get_format_one_line_size(amount);
  uint64_t ending = start + line_size;
  if (ending > file_size){ return NO_BYTES_READ; }
  for (int j = 0; j < amount; j++){
    if (*(file++) == ' '){ return NO_BYTES_READ; }
    file += FORMAT_ONE_NUM_SIZE;
  }
  return (file - start); // return number of bytes read
}

uint16_t get_format_one_line_size(uint16_t amount){
  uint8_t space_size = 1;
  return space_size + sizeof(amount) + space_size
  + amount * FORMAT_ONE_NUM_SIZE + (space_size * amount - 1);
}


uint16_t read_format_two(unsigned char* file, uint64_t file_size){
  uint64_t start = (uint64_t)file;
  if (*(file++) != ' ') { return NO_BYTES_READ;}
  uint32_t amount = read_format_two_amount(file);
  file += FORMAT_TWO_AMOUNT_SIZE;
  if (*(file++) != ' ' ) { return NO_BYTES_READ; }
  for (int j = 0; j < amount; j++){
    uint8_t bytes_read = parse_format_two_num(file);
    file += bytes_read;
    if (*(file++) != ',') { return NO_BYTES_READ };
  }
  return file - start;
}


uint16_t to_int16(uint8_t greater_bits, uint8_t lower_bits){
  uint16_t number = greater_bits;
  return (number << 8) | lower_bits;
}

uint32_t read_format_two_amount(unsigned char* file){
  char amount_as_str[FORMAT_TWO_AMOUNT_SIZE];
  memcpy(amount_as_str, file, FORMAT_TWO_AMOUNT_SIZE);
  return atoi(amount_as_str);
}

uint8_t parse_format_two_num(unsigned char* file){
  unsigned char* start = file;
	while (!done_parsing_num(start, file++)) {}
	return file - start;
}


bool done_parsing_num(unsigned char* start, unsigned char* curr_pos){
  return !isdigit(*curr_pos) || (curr_pos - start) < FORMAT_TWO_NUM_SIZE;
}


