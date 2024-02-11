#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* utf8_string_to_hex_string(const char* str) {
	// Check if encoding is needed by examining each character in the input string

	//Проверка на кодировку не имеет смысла. Если мы вызываем эту функцию, значит кодировка нужна 
	/*
	int is_encoded = 0;

	for (int i = 0; str[i] != '\0'; ++i) {
		if (str[i] < 32 || str[i] > 126) {
			is_encoded = 1;
			break;
		}
	}

	if (is_encoded == 0) {
		return NULL;
	}
	*/
	// If encoding is needed, create a new string for the hexadecimal representation

	// Calculate the length of the new string
	int len = 0;
	for (int i = 0; str[i] != '\0'; ++i) {
		len += 4; // Two hexadecimal characters and two characters for "="
	}

	// Allocate memory for the new string
	char* encoded_str = (char*)calloc(len + 1, sizeof(char)); // +1 for the null terminator

	if (encoded_str == NULL) {
		// Memory allocation failed
		printf("Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}

	// Encode each character in the input string to hexadecimal and append "="
	int index = 0;
	for (int i = 0; str[i] != '\0'; ++i) {
		// Format the hexadecimal representation and append "="
		sprintf(encoded_str + index, "=%02x", (unsigned char)str[i]);

		char w1 = encoded_str[index + 1];
		if (w1 >= 97 && w1 <= 122) {
			encoded_str[index + 1] = w1 - 32;
		}

		w1 = encoded_str[index + 2];
		if (w1 >= 97 && w1 <= 122) {
			encoded_str[index + 2] = w1 - 32;
		}

		index += 3; // Move to the next position for the next iteration
	}

	return encoded_str;
}

#define _VCFE_ENCODE_TO_HEX_C_OBSOLETE
#ifndef _VCFE_ENCODE_TO_HEX_C_OBSOLETE

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vcfe_encode_to_hex.h"

int find_first_zero(int32_t* array, int array_size) {
	int max = array_size;
	int min = 0;

	while (min < max) {
		int mid = min + (max - min) / 2;

		if (array[mid] == 0) {
			max = mid;
		}
		else {
			min = mid + 1;
		}
	}

	// Check if min is within array bounds and if the element at min is zero
	if (min < array_size && array[min] == 0) {
		return min;
	}
	else {
		// No zero found in the array
		int32_t* check = realloc(array, sizeof(int32_t) * array_size * 2);
		if (check == NULL) {
			return -1;
		}
	}

	return -1;
}
///////////////////////////////////////////////

int wchar_to_hexnums(wchar_t utf8_char, int32_t* hexnums, int hexnums_size) {
	int last_zero = find_first_zero(hexnums, hexnums_size);

	if (last_zero == -1) {
		return -1;
	}

	if (utf8_char <= 127) {
		hexnums[last_zero] = (int32_t)utf8_char;
		return last_zero + 1;
	}

	int bit_size;
	if (utf8_char <= 2047) {
		bit_size = 2;
	}
	else if (utf8_char <= 65535) {
		bit_size = 3;
	}
	else {
		return last_zero + 1;
	}

	uint32_t six_bits_set = 0b0000'0000'0000'0000'0000'0000'0011'1111;
	uint32_t six_bits_f = utf8_char & six_bits_set;
	uint32_t bitmask = 0b1000'0000;

	switch (bit_size) {
	case 2:

		six_bits_set = 0b0000'0000'0000'0000'0000'0000'0011'1111;
		six_bits_f = utf8_char & six_bits_set;
		bitmask = 0b1000'0000;
		hexnums[last_zero + 1] = six_bits_f | bitmask;

		six_bits_set = 0b0000'0000'0000'0000'0000'1111'1100'0000;
		six_bits_f = utf8_char & six_bits_set;

		bitmask = 0b1100'0000;
		hexnums[last_zero] = (six_bits_f >> 6) | bitmask;

		return last_zero + 2;
	case 3:

		six_bits_set = 0b0000'0000'0000'0000'0000'0000'0011'1111;
		six_bits_f = utf8_char & six_bits_set;
		bitmask = 0b1000'0000;
		hexnums[last_zero + 2] = six_bits_f | bitmask;

		six_bits_set = 0b0000'0000'0000'0000'0000'1111'1100'0000;
		six_bits_f = utf8_char & six_bits_set;
		bitmask = 0b1100'0000;
		hexnums[last_zero + 1] = (six_bits_f >> 6) | bitmask;

		six_bits_set = 0b0000'0000'0000'0011'1111'0000'0000'0000;
		six_bits_f = utf8_char & six_bits_set;
		bitmask = 0b1110'0000;
		hexnums[last_zero] = (six_bits_f >> 12) | bitmask;

		return last_zero + 3;
	default:
		return last_zero + 1;
	}
}
///////////////////////////////////////////

int32_t* utf8_to_hexnums(const wchar_t* hex_string, int* hex_end) {
	int hex_string_size = (int)wcslen(hex_string) + 1;
	int hexnums_size = hex_string_size * 4 + 1;

	int32_t* hexnums = (int32_t*)calloc(hexnums_size, sizeof(int32_t));

	if (hexnums == NULL) {
		return NULL;
	}

	*hex_end = 0;
	for (int i = 0; i < hex_string_size; i++) {
		*hex_end = wchar_to_hexnums(hex_string[i], hexnums, hexnums_size);
	}

	if (*hex_end == 0) {
		free(hexnums);
		return NULL;
	}

	int32_t* check = realloc(hexnums, *hex_end * sizeof(int32_t));

	if (check == NULL) {
		return NULL;
	}

	return hexnums;
}

///////////////////////////////////////////

char* utf8_string_to_hex_string(const wchar_t* utf8_string) {
	int hex_end = 0;
	int32_t* hexnums = utf8_to_hexnums(utf8_string, &hex_end);

	char* hex_string = (char*)calloc(hex_end * 3 + 1, sizeof(char));

	if (hex_string == NULL) {
		return NULL;
	}

	int i = 0;
	for (; hexnums[i] != 0; ++i) {
		char block[3] = { 0 };
		snprintf(block, 3, "%X", hexnums[i]);

		size_t string_end = strlen(hex_string);
		hex_string[string_end] = block[0];
		hex_string[string_end + 1] = block[1];
		hex_string[string_end + 2] = '=';
	}

	free(hexnums);

	return hex_string;
}

///////////////////////////////////////////

#ifdef VCF_DEBUG_ONLY

void utf8_to_hexnums_TEST(const wchar_t* hex_string) {
	int hex_end = 0;
	int32_t* hexnums = utf8_to_hexnums(hex_string, &hex_end);
	int max_size = (int)wcslen(hex_string) * 4 + 1;
	int i = 0;
	while (1) {
		if (hexnums[i] == 0) {
			printf("0\n");
			break;
		}
		printf("%x, ", hexnums[i]);
		i++;
		if (i > max_size) {
			break;
		}
	}

	free(hexnums);
}

void utf8_string_to_hex_string_TEST(void) {
	const wchar_t* utf8_string = L"Бабушка-лапушка сказала: <<UTF-8就是为了解决向后兼容ASCII码而设计，Unicode中前128个字符，使用与ASCII码相同的二进制值的单个字节进行编码，而且字面与ASCII码的字面一一对应，這使得原來處理ASCII字元的軟體無須或只須做少部份修改，即可繼續使用。!!>>$£ह한   ";

	char* hex_string = utf8_string_to_hex_string(utf8_string);

	printf("\n\n%s\n\n", hex_string);

	free(hex_string);
	}

#endif // VCF_DEBUG_ONLY

#endif // _VCFE_ENCODE_TO_HEX_C_OBSOLETE