//#define TESTCLANG
#ifdef TESTCLANG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vcfe_deencode_funcs.h"

int main(void) {
	system("chcp 1251 >NUL");

	char inputString[] = "=D0=91=D0=BE=D1=80=D1=8F=20=D0=90=D0=BA=20=D0=92=D0=B8=D1=88=D0=BD=D0=B5=D0=B2=D0=B5=D1=86=D0=BA=D0=B8=D0=B9";

	char* decodedString = (char*)calloc(strlen(inputString), sizeof(char));

	decode_hex_string(inputString, decodedString);

	printf("%s\n", decodedString);

	free(decodedString);

	////////////////////////////////

	char input[] = "ßßß";

	char* encodedString = (char*)calloc(strlen(input) * 6 + 1, sizeof(char));

	encode_hex_string(input, encodedString);

	printf("%s\n", encodedString);

	free(encodedString);

	return 0;
}

#endif // TESTCLANG