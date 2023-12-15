#include <string.h>
#include <stdlib.h>
#include "vcfe_code_dict.h"
#include "vcfe_encode_symbol.h"
#include "vcfe_deencode_funcs.h"

/// /// /// /// /// /// /// /// /// /// /// ///

int count_leading_ones(const char* hexNumber) {
	// Convert hex to decimal
	unsigned long decimalNumber = strtoul(hexNumber, NULL, 16);

	int count = 0;

	// Iterate through each bit of the number
	while ((decimalNumber & (1 << (7 - count))) != 0) {
		count++;
	}

	return count;
}

/// /// /// /// /// /// /// /// /// /// /// ///

void decode_hex_string111(const char* name, char* result_string) {
	int name_size = strlen(name);

	if (!result_string || !name_size) {
		return;
	}

	int index = 0;
	while (1) {
		if (name[index] == '=' || name[index + 1] == NULL) {
			++index;
			continue;
		}

		char pair[2] = { name[index], name[index + 1] };

		int bit_size = count_leading_ones(pair);

		switch (bit_size) {
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		default:
			break;
		}
	}
}

void decode_hex_string(const char* name, char* result_string) {
	int name_size = strlen(name);

	if (!result_string || !name_size) {
		return;
	}

	int index = 0;

	int dict_size = sizeof(HexDictionary) / sizeof(HexDictionary[0]);

	int char_number = 0;

	while (index < name_size) {
		int found = 0;

		for (int i = 0; i < dict_size; ++i) {
			DictPairs dp = HexDictionary[i];

			size_t key_size = strlen(dp.key);

			int sss = strncmp(name + index, dp.key, key_size);

			if (!sss) {
				result_string[char_number] = dp.letter;
				++char_number;
				index += key_size;
				found = 1;
				break;
			}
		}

		if (!found) {
			result_string[char_number] = '?';
			++char_number;
			index += (name[index + 1] == 'D') ? 6 : 3;
		}
	}

	result_string[char_number] = '\0';

	return;
}

/// /// /// /// /// /// /// /// /// /// /// ///

void encode_hex_string(const char* name, char* result_string) {
	int index = 0;
	int name_size = strlen(name);

	if (!result_string || !name_size) {
		return;
	}

	char encodedString[7];

	for (int i = 0; i != name_size; ++i) {
		encode_symbol(name[i], encodedString);

		if (!encodedString) {
			strcat(result_string, "=3F");
			index += 3;
			break;
		}

		strcat(result_string, encodedString);
	}

	return;
}