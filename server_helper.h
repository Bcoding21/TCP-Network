#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> /* misc. UNIX functions */

uint8_t read_trans_type(int);

uint64_t read_file_size(int);

char* read_file(int, uint64_t);

uint16_t read_file_name_length(int);

char* read_file_name(int, uint16_t);