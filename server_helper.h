#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h> 

#define FORMAT_ONE_TYPE 0
#define FORMAT_TWO_TYPE 1
#define FORMAT_ONE_NUM_SIZE 2 // bytes
#define FORMAT_TWO_NUM_SIZE 5 // characters
#define FORMAT_ONE_AMOUNT_SIZE 1 // bytes
#define FORMAT_TWO_AMOUNT_SIZE 3 // bytes
#define NO_BYTES_READ -1

struct Message {
    uint8_t trans_type;
    uint64_t file_size;
    unsigned char* file;
    uint64_t name_length;
    unsigned char* file_name;
};

uint8_t read_trans_type(int);

uint64_t read_file_size(int);

unsigned char* read_file(int, uint64_t);

uint16_t read_file_name_length(int);

char* read_file_name(int, uint16_t);

bool is_good_format(unsigned char*, unsigned char*);

bool is_type(uint8_t);

bool is_end_of_line(unsigned char);

bool is_end_of_number(unsigned char);

/**
 * Returns number of bytes a line contains
 * that is written in the first format.
 * */
uint16_t get_format_one_byte_count(unsigned char*, unsigned char*);

/**
 * Returns number of bytes a line contains
 * that is written in the first format.
 * */
uint16_t get_format_two_byte_count(unsigned char*, unsigned char*);

uint16_t to_int16(uint8_t, uint8_t);

uint16_t get_format_one_length(uint8_t);

uint32_t read_format_two_amount(unsigned char*);

uint32_t get_str_as_int32(unsigned char*, unsigned char*);

/**
 * Returns the number of bytes in a 
 * format one line given the amount 
 * provided in that line.
 * */
uint16_t calculate_format_one_len(uint16_t);

/**
 * Returns the number of bytes a number
 * that is represented as ascii has.
 * */
uint8_t get_format_two_num_size(unsigned char*);

bool done_parsing_num(unsigned char*, unsigned char*);

unsigned char* remove_non_print_chars(unsigned char*, uint64_t);

