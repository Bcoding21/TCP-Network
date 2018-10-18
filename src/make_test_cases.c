#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// command line arguement indexe
#define NUM_ROWS 1000000
#define NUMS_PER_ROW 20
#define FORMAT_ONE 1
#define FORMAT_TWO 2
#define BOTH_FORMATS 3
#define FORMAT_ONE_TYPE 0
#define FORMAT_TWO_TYPE 1
#define FORMAT_TWO_AMOUNT_SIZE 3
#define FORMAT_TWO_NUM_SIZE 5
#define DIR "../testData/"

void write_format_one_test_file(FILE*);

void write_format_two_test_file(FILE*);

void write_both_format_test_file(FILE*);

char* amount_to_str(uint8_t);

char* num_to_str(uint16_t);

bool file_exist(char*);

uint8_t get_num_digits(uint32_t);

void format_one_errors(FILE*);

void format_two_errors(FILE*);

void both_with_format_one_errors(FILE*);

void both_with_format_two_errors(FILE*);

void all_two_error(FILE*);

int main(int argc, char* argv[]){

  srand(time(0));

struct stat st = {0};

if (stat("../testData/", &st) == -1) {
    mkdir("../testData/", 0700);
}

printf("Int16 max: %d", INT16_MAX);
  FILE* file = fopen("../testData/all_format_one.bin", "w");
  write_format_one_test_file(file);
  fclose(file);

  file = fopen("../testData/all_format_two.bin", "w");
  write_format_two_test_file(file);
  fclose(file);

  file = fopen("../testData/both_formats.bin", "w");
  write_both_format_test_file(file);
  fclose(file);

  file = fopen("../testData/all_format_one_errors.bin", "w");
  format_one_errors(file);
  fclose(file);

  file = fopen("../testData/all_format_two_errors.bin", "w");
  format_two_errors(file);
  fclose(file);

  file = fopen("../testData/both_format_one_errors.bin", "w");
  both_with_format_one_errors(file);
  fclose(file);

  file = fopen("../testData/both_format_two_errors.bin", "w");
  both_with_format_two_errors(file);
  fclose(file);

  return 0;
}

void write_format_one_test_file(FILE* file){
	for (int i = 0; i < NUM_ROWS; i++) {
		uint8_t format = FORMAT_ONE_TYPE;
		fwrite(&format, sizeof(format), 1, file);
		uint8_t amount = NUMS_PER_ROW;
		fwrite(&amount, sizeof(amount), 1, file);
		for (int i = 0; i < amount; i++) {
			uint16_t rand_num = rand() % INT16_MAX;
			fwrite(&rand_num, sizeof(rand_num), 1, file);
		}
	}
}

void write_format_two_test_file(FILE* file) {
	for (int i = 0; i < NUM_ROWS; i++) {
		uint8_t format = FORMAT_TWO_TYPE;;
		fwrite(&format, sizeof(format), 1, file);
		uint8_t amount = NUMS_PER_ROW;
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

void write_both_format_test_file(FILE* file) {
	for (int i = 0; i < NUM_ROWS; i++) {
		uint8_t type = i % 2 == 0;
		fwrite(&type, sizeof(type), 1, file);
		if (type == FORMAT_ONE_TYPE) {
			uint8_t amount = NUMS_PER_ROW;
			fwrite(&amount, sizeof(amount), 1, file);
			for (int i = 0; i < amount; i++) {
				uint16_t rand_num = rand() % INT16_MAX;
				fwrite(&rand_num, sizeof(rand_num), 1, file);
			}
		}
		else {
			uint8_t amount = NUMS_PER_ROW;
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
}

void format_one_errors(FILE* file) {
	for (int i = 0; i < NUM_ROWS; i++) {
		uint8_t format = FORMAT_ONE_TYPE;
		fwrite(&format, sizeof(format), 1, file);
		uint8_t amount = NUMS_PER_ROW;
		fwrite(&amount, sizeof(amount), 1, file);
		for (int i = 0; i < amount; i++) {
			uint32_t rand_num = rand() % INT32_MAX + INT16_MAX;
			fwrite(&rand_num, sizeof(rand_num), 1, file);
		}
	}
}

void format_two_errors(FILE* file) { // error but putting comma after the last number
	for (int i = 0; i < NUM_ROWS; i++) {
		uint8_t format = FORMAT_TWO;
		fwrite(&format, sizeof(format), 1, file);
		uint8_t amount = NUMS_PER_ROW;
		char* str_amount = amount_to_str(amount);
		fputs(str_amount, file);
		free(str_amount);
		for (int i = 0; i < amount; i++) {
			uint16_t rand_num = rand() % INT16_MAX;
			char* str_num = num_to_str(rand_num);
			fputs(str_num, file);
			fputc(',', file);
			free(str_num);
		}
	}
}

void both_with_format_one_errors(FILE* file) { // error by amonut being too many bytes
	for (int i = 0; i < NUM_ROWS; i++) {
		uint8_t type = i % 2 == 0;
		fwrite(&type, sizeof(type), 1, file);
		if (type == FORMAT_ONE_TYPE) {
			uint8_t amount = NUMS_PER_ROW;
			fwrite(&amount, sizeof(amount), 1, file);
			for (int i = 0; i < amount; i++) {
				uint32_t rand_num = rand() % INT32_MAX + INT16_MAX;
				fwrite(&rand_num, sizeof(rand_num), 1, file);
			}
		}
		else {
			uint8_t amount = NUMS_PER_ROW;
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
}

void both_with_format_two_errors(FILE* file) { // error by adding too many commas
	for (int i = 0; i < NUM_ROWS; i++) {
		uint8_t type = i % 2 == 0;
		fwrite(&type, sizeof(type), 1, file);
		if (type == FORMAT_ONE_TYPE) {
			uint8_t amount = NUMS_PER_ROW;
			fwrite(&amount, sizeof(amount), 1, file);
			for (int i = 0; i < amount; i++) {
				uint32_t rand_num = rand() % INT32_MAX + INT16_MAX;
				fwrite(&rand_num, sizeof(rand_num), 1, file);
			}
		}
		else {
			uint8_t amount = NUMS_PER_ROW;
			char* str_amount = amount_to_str(amount);
			fputs(str_amount, file);
			free(str_amount);
			for (int i = 0; i < amount; i++) {
				uint16_t rand_num = rand() % INT16_MAX;
				char* str_num = num_to_str(rand_num);
				fputs(str_num, file);
				fputc(',', file);
				free(str_num);
			}
		}
	}
}