#include "server_helper.h"


int create_server(uint16_t port_number){
  int server = 0;
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		puts("SOCKET FAILURE");
	}

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port_number);


	if (bind(server, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		puts("BIND FAILURE");
	}
  return server;
}

void run_server(int server_socket_fd) {
  if (listen(server_socket_fd, 3) < 0) {
		puts("LISTEN ERROR");
	}
  	struct sockaddr_in client_address;
		int address_size = sizeof(client_address);
		int client = 0;

  	while (1) {
		if ((client = accept(server_socket_fd, (struct sockaddr*) &client_address, &address_size)) < 0) {
			puts("ACCEPT ERROR");
		}
		else {
			Message message = read_message(client);
      unsigned char* file_begin = message.file;
      unsigned char* file_end = message.file + message.file_size;
      bool is_ok = is_good_format(file_begin, file_end);
			if (is_ok){
				send(client, SUCCESS_MESSAGE, sizeof(SUCCESS_MESSAGE), NO_FLAGS);
			}
			else{
				send(client, ERROR_MESSAGE, sizeof(ERROR_MESSAGE), NO_FLAGS);
			}
		}
		if (close(client) < 0) {
			puts("CONNECTION CLOSE EROOR");
		}
	}
}

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
  return 0;
}

char* read_file_name(int socket_fd, uint16_t name_length){
  char* new_name = malloc(name_length + 1);
  new_name[name_length] = '\0';
  read(socket_fd, new_name, name_length);
  return new_name;
}


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

Message read_message(int socket_fd){
  Message message;
  message.trans_type = read_trans_type(socket_fd);
  printf("Trans type: %d\n", message.trans_type);
  message.file_size = read_file_size(socket_fd);
  printf("File size: %d\n", message.file_size);
  message.file = read_file(socket_fd, message.file_size);
  printf("File content: %s\n", message.file);
  message.name_length = read_file_name_length(socket_fd);
  message.file_name = read_file_name(socket_fd, message.name_length);
  fflush(stdout);
  return message;
}

