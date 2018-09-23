 /* inet (3) funtions */
#include "server_helper.h"

char* get_file(char* file_name) {
	uint64_t file_size = get_file_size(file_name);
	char* buffer = malloc(file_size + 1);
	buffer[file_size] = '\0';
	FILE* file = fopen(file_name, "rb");
	if (file == NULL) {
		puts("ERROR READING FILE");
		exit(-1);
	}
	fread(buffer, sizeof(char), file_size, file);
	return buffer;
}

int get_file_size(char* file_name) {
	FILE* file = fopen(file_name, "rb");
	fseek(file, 0L, SEEK_END);
	long size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	return size;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	uint16_t port_number = atoi(argv[1]);

	int socket_server_fd = create_server(port_number);

	run_server(socket_server_fd);

	if (close(socket_server_fd) < 0){
		puts("ERROR CLOSING SERVER");
	}
}
