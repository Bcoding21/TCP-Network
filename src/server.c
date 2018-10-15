 /* inet (3) funtions */
#include "server_helper.h"


int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	int socket_server_fd = create_server(atoi(argv[1]));
	
	run_server(socket_server_fd);

	if (close(socket_server_fd) < 0){
		puts("ERROR CLOSING SERVER");
	}
}
