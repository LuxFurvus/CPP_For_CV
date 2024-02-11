#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdint.h>

char* hex_string_to_utf8_string(const char* str) {
	int str_size = (int)strlen(str);

	int number_of_hex = (str_size / 3) + 1;

	uint32_t* hexnums = (uint32_t*)calloc(number_of_hex, sizeof(uint32_t));

	char pair[3] = { 0 };

	int h = 0;

	for (int i = 0; i < str_size; ++i) {
		if (str[i] == '=') {
			continue;
		}
		pair[0] = str[i];
		pair[1] = str[i + 1];

		hexnums[h] = strtol(pair, NULL, 16);
		++h;
		++i;
	}

	char* utf8_string = (char*)calloc(number_of_hex + 1, 1);

	for (int i = 0; i < number_of_hex; ++i) {
		utf8_string[i] = (char)hexnums[i];
	}

	free(hexnums);

	return utf8_string;
}

#ifdef VCFE_ENDECODE_FUNC_H_OBSOLETE

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include "vcfe_decode_from_hex.h"

uint32_t* hex_string_to_hexnums(const char* str) {
	int str_size = (int)strlen(str);

	int number_of_hex = (str_size / 3) + 1;

	uint32_t* hexnums = (uint32_t*)calloc(number_of_hex, sizeof(uint32_t));

	char pair[3] = { 0 };

	int h = 0;

	for (int i = 0; i < str_size; ++i) {
		if (str[i] == '=') {
			continue;
		}
		pair[0] = str[i];
		pair[1] = str[i + 1];

		hexnums[h] = strtol(pair, NULL, 16);
		++h;
		++i;
	}

	return hexnums;
}

wchar_t hexnums_to_wchar(const uint32_t hex[3], const int bit_size) {
	if (hex[0] == 0) {
		return L'\0';
	}
	int bitmask;
	int pre_result[3];

	switch (bit_size) {
	default:
	case 0:
	case 1:
		return (wchar_t)hex[0];
		break;
	case 2:
		bitmask = 0b0001'1111;
		pre_result[0] = (hex[0] & bitmask) << 6;
		//
		bitmask = 0b0011'1111;
		pre_result[1] = hex[1] & bitmask;
		return (wchar_t)(pre_result[0] | pre_result[1]);
		break;
	case 3:
		bitmask = 0b0001'1111;
		pre_result[0] = (hex[0] & bitmask) << 12;
		//
		bitmask = 0b0011'1111;
		pre_result[1] = (hex[1] & bitmask) << 6;
		pre_result[2] = hex[2] & bitmask;
		return (wchar_t)(pre_result[0] | pre_result[1] | pre_result[2]);
		break;
	}
}

wchar_t* hex_string_to_utf8_string(const char* hexString) {
	int str_size = (int)strlen(hexString) + 1;

	int number_of_hex = (str_size / 3) + 1;

	uint32_t* hexnums = hex_string_to_hexnums(hexString);

	wchar_t* utf8_str = (wchar_t*)calloc(str_size, sizeof(wchar_t));

	if (hexnums == NULL || utf8_str == NULL) {
		return NULL;
	}

	int i = 0;
	int u = 0;
	while (i < number_of_hex) {
		int count = 0;
		int to_literal_print = 0;
		if (hexnums[i] <= 127) {//till the last 1-bit char
			to_literal_print = 0;
			count = 1;
		}
		else if (hexnums[i] < 194) {//between 1-bit & 2-bit groups
			to_literal_print = 1;
			count = 1;
		}
		else if (hexnums[i] <= 223) {//till the last 2-bit char
			to_literal_print = 0;
			count = 2;
		}
		else if (hexnums[i] <= 239) {//till the last 3-bit char
			to_literal_print = 0;
			count = 3;
		}
		else {//till the last 4-bit char
			to_literal_print = 1;
			count = 4;
		}

		//This part is intended for hex-numbers, that have no UTF-8 representation
		//While parsing normal VCF this part is not used
		if (to_literal_print) {
			for (int k = 0; k < count; k++) {
				wchar_t pair[3] = { 0 };
				utf8_str[u] = L'=';
				swprintf(pair, 3, L"%X", hexnums[i++]);
				wcscat(utf8_str, pair);
				u = u + 3;
			}
			continue;
		}

		uint32_t hexes[3] = { 0 };

		switch (count) {
		default:
		case 0:
		case 1:
			hexes[0] = hexnums[i];
			utf8_str[u++] = hexnums_to_wchar(hexes, 1);
			++i;
			break;
		case 2:
			hexes[0] = hexnums[i];
			hexes[1] = hexnums[++i];
			utf8_str[u++] = hexnums_to_wchar(hexes, 2);
			++i;
			break;
		case 3:
			hexes[0] = hexnums[i];
			hexes[1] = hexnums[++i];
			hexes[2] = hexnums[++i];
			utf8_str[u++] = hexnums_to_wchar(hexes, 3);
			++i;
			break;
		}
	}

	free(hexnums);

	return utf8_str;
}
//////////////////////////////////////////

#ifdef VCF_DEBUG_ONLY

void hex_string_to_utf8_string_TEST(void) {
	const char* hex_string = "D0=91=D0=B0=D0=B1=D1=83=D1=88=D0=BA=D0=B0=2D=D0=BB=D0=B0=D0=BF=D1=83=D1=88=D0=BA=D0=B0=20=D1=81=D0=BA=D0=B0=D0=B7=D0=B0=D0=BB=D0=B0=3A=20=3C=3C=55=54=46=2D=38=E5=F0=B1=E6=D8=AF=E4=F8=BA=E4=FA=86=E8=E7=A3=E5=C6=B3=E5=D0=91=E5=D0=8E=E5=C5=BC=E5=EE=B9=41=53=43=49=49=E7=E0=81=E8=C0=8C=E8=EE=BE=E8=EE=A1=EF=FC=8C=55=6E=69=63=6F=64=65=E4=F8=AD=E5=C9=8D=31=32=38=E4=F8=AA=E5=ED=97=E7=EC=A6=EF=FC=8C=E4=FD=BF=E7=D4=A8=E4=F8=8E=41=53=43=49=49=E7=E0=81=E7=DB=B8=E5=D0=8C=E7=DA=84=E4=FA=8C=E8=FF=9B=E5=C8=B6=E5=C0=BC=E7=DA=84=E5=CD=95=E4=F8=AA=E5=ED=97=E8=CA=82=E8=FF=9B=E8=E1=8C=E7=FC=96=E7=E0=81=EF=FC=8C=E8=C0=8C=E4=F8=94=E5=ED=97=E9=DD=A2=E4=F8=8E=41=53=43=49=49=E7=E0=81=E7=DA=84=E5=ED=97=E9=DD=A2=E4=F8=80=E4=F8=80=E5=EF=B9=E5=FA=94=EF=FC=8C=E9=C0=99=E4=FD=BF=E5=FE=97=E5=CE=9F=E4=FE=86=E8=D9=95=E7=D0=86=41=53=43=49=49=E5=ED=97=E5=C5=83=E7=DA=84=E8=FB=9F=E9=EB=94=E7=C4=A1=E9=E0=88=E6=C8=96=E5=CF=AA=E9=E0=88=E5=C1=9A=E5=F0=91=E9=C3=A8=E4=FB=BD=E4=FF=AE=E6=D4=B9=EF=FC=8C=E5=CD=B3=E5=CF=AF=E7=F9=BC=E7=FA=8C=E4=FD=BF=E7=D4=A8=E3=C0=82=21=21=3E=3E=24=C2=A3=E0=E4=B9=ED=D5=9C=20=20=20=";

	wchar_t* utf8_str = hex_string_to_utf8_string(hex_string);

	wprintf(L"\n%ls***\n", utf8_str);

	free(utf8_str);
}

#endif // VCF_DEBUG_ONLY

#endif // VCFE_ENDECODE_FUNC_H