 /* inet (3) funtions */
#include "server_helper.h"


int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(1);
	}

	uint16_t port_number = atoi(argv[1]);

	int socket_server_fd = create_server(port_number);

	run_server(socket_server_fd);

}
