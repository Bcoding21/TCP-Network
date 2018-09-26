#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_LINES 3
#define MAX_NUMS_PER_ROW 3
#define NUM_ARGS 6
// command line arguement indexes
#define FORMAT 1 // 1 - all format 1, 2 - all format2 , 3 - both formats
#define NUM_ROWS 2
#define MAX_NUMBERS_PER_ROW 3
#define OUTPUT_FILE 4
#define HAS_ERROR 5
#define FORMAT_ONE 1
#define FORMAT_TWO 2
#define BOTH_FORMATS 3
#define FORMAT_ONE_TYPE 0
#define FORMAT_TWO_TYPE 1
#define FORMAT_TWO_AMOUNT_SIZE 3
#define FORMAT_TWO_NUM_SIZE 5;


void write_format_one_test_file(char*, uint64_t, uint64_t);

void write_format_two_test_file(char*, uint64_t, uint64_t);

void write_both_format_test_file(char*, uint64_t, uint64_t);

char* amount_to_str(uint32_t);

char* num_to_str(uint16_t);

bool file_exist(char*);

uint8_t get_num_digits(uint32_t);

int main(int argc, char* argv[]){

  if (argc < NUM_ARGS){
    puts("NEED FILE PATH TO OUTPUT");
    exit(1);
  }

  srand(time(0));

  uint8_t format = atoi(argv[FORMAT]);
  uint64_t num_rows = atoi(argv[NUM_ROWS]);
  uint64_t nums_per_row = atoi(argv[MAX_NUMBERS_PER_ROW]);
  char* file_path = argv[OUTPUT_FILE];
  bool has_error = atoi(argv[HAS_ERROR]);

  switch (format) {
    case FORMAT_ONE:
		write_format_one_test_file(file_path, num_rows, nums_per_row);
		break;
    case FORMAT_TWO:
		write_format_two_test_file(file_path, num_rows, nums_per_row);
		break;
    case BOTH_FORMATS:
		write_both_format_test_file(file_path, num_rows, nums_per_row);
  }

  return 0;
}


bool file_exist(char* file_name){

}



void write_format_one_test_file(char* file_name, uint64_t num_rows, uint64_t nums_per_row){
	FILE* file = fopen(file_name, "wb");
	for (int i = 0; i < num_rows; i++) {
		uint8_t format = FORMAT_ONE_TYPE;
		fwrite(&format, sizeof(format), 1, file);
		uint8_t amount = rand() % nums_per_row + 1;
		fwrite(&amount, sizeof(amount), 1, file);
		for (int i = 0; i < amount; i++) {
			uint16_t rand_num = rand() % INT16_MAX;
			fwrite(&rand_num, sizeof(rand_num), 1, file);
		}
	}
	fclose(file);
}

void write_format_two_test_file(char* file_name, uint64_t num_rows, uint64_t nums_per_row) {
	FILE* file = fopen(file_name, "w");
	for (int i = 0; i < num_rows; i++) {
		uint8_t format = FORMAT_TWO;
		fwrite(&format, sizeof(format), 1, file);
		uint8_t amount = rand() % nums_per_row + 1;
		char* str_amount = amount_to_str(amount);
		fputs(str_amount, file);
		free(str_amount);
		for (int i = 0; i < amount - 1; i++) {
			uint16_t rand_num = rand() % INT16_MAX;
			char* str_num = num_to_str(rand_num);
			fputs(str_num, file);
			fputc(',', file);
			free(str_num);
		}
		uint16_t rand_num = rand() % INT16_MAX;
		char* str_num = num_to_str(rand_num);
		fputs(str_num, file);
		free(str_num);
	}
	fclose(file);
}


char* amount_to_str(uint8_t amount) {
	char* str_amount = malloc(FORMAT_TWO_AMOUNT_SIZE + 1);
	str_amount[FORMAT_TWO_AMOUNT_SIZE] = '\0';
	memset(str_amount, '0', FORMAT_TWO_AMOUNT_SIZE);
	uint8_t num_digits = get_num_digits(amount);
	int digit_index = FORMAT_TWO_AMOUNT_SIZE - 1;
	for (int i = 0; i < num_digits; i++) {
		str_amount[digit_index--] = (amount % 10) + '0';
		amount /= 10;
	}
	return str_amount;
}

uint8_t get_num_digits(uint32_t number) {
	uint8_t digit_count = 0;
	while (number != 0) {
		digit_count++;
		number /= 10;
	}
	return digit_count;
}


char* num_to_str(uint16_t number) {
	uint8_t num_digits = get_num_digits(number);
	char* str_num = malloc(num_digits + 1);
	str_num[num_digits] = '\0';
	for (int i = num_digits - 1; i > -1; i--) {
		str_num[i] = (number % 10) + '0';
		number /= 10;
	}
	return str_num;
}

void write_both_format_test_file(char* file_name, uint64_t num_rows, uint64_t nums_per_row) {
	FILE* file = fopen(file_name, "w");
	for (int i = 0; i < num_rows; i++) {
		uint8_t type = i % 2 == 0;
		fwrite(&type, sizeof(type), 1, file);
		uint8_t amount = rand() % nums_per_row + 1;
		if (type == FORMAT_ONE_TYPE) {
			fwrite(&amount, sizeof(amount), 1, file);
			for (int i = 0; i < amount; i++) {
				uint16_t rand_num = rand() % INT16_MAX;
				fwrite(&rand_num, sizeof(rand_num), 1, file);
			}
		}
		else {
			char* str_amount = amount_to_str(amount);
			fputs(str_amount, file);
			free(str_amount);
			for (int i = 0; i < amount - 1; i++) {
				uint16_t rand_num = rand() % INT16_MAX;
				char* str_num = num_to_str(rand_num);
				fputs(str_num, file);
				fputc(',', file);
				free(str_num);
			}
			uint16_t rand_num = rand() % INT16_MAX;
			char* str_num = num_to_str(rand_num);
			fputs(str_num, file);
			free(str_num);
		}
	}
	fclose(file);
}
