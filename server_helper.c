
#include "server_helper.h"


uint8_t read_trans_type(int socket_fd){
  uint8_t trans_type;
  read(socket_fd, &trans_type, sizeof(trans_type));
  return trans_type;
}

uint64_t read_file_size(int socket_fd){
  uint64_t file_size;
  read(socket_fd, &file_size, sizeof(file_size));
  return 0;
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
  return 0;
}

char* read_file_name(int socket_fd, uint16_t name_length){
  char* new_name = malloc(name_length + 1);
  new_name[name_length] = '\0';
  read(socket_fd, new_name, name_length);
  return new_name;
}

/**
 * 
 * */
bool is_good_format(unsigned char* curr_file_pos, unsigned char* end){
  while (curr_file_pos < end) {
    uint8_t type = *curr_file_pos++;
	if (type == (uint8_t)'\n') { continue; }
	if (!is_type(type)) { return false; }
    int16_t bytes_read = (type == FORMAT_ONE_TYPE) ? 
    get_format_one_byte_count(curr_file_pos, end) :
    get_format_two_byte_count(curr_file_pos, end) ;
    if (bytes_read == NO_BYTES_READ) { return false; }
    curr_file_pos += bytes_read;
  }
  return curr_file_pos == end;
}

bool is_type(unsigned char type){
  return type == FORMAT_ONE_TYPE || type == FORMAT_TWO_TYPE;
}

uint16_t get_format_one_byte_count(unsigned char* curr_file_pos, unsigned char* file_end){
  uint8_t amount = *curr_file_pos;
  unsigned char* line_end = curr_file_pos + get_format_one_length(amount);
  return (line_end > file_end) ? NO_BYTES_READ : (line_end - curr_file_pos);
}

uint16_t get_format_one_length(uint8_t amount){
  return  sizeof(amount) + amount * FORMAT_ONE_NUM_SIZE;
}

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

