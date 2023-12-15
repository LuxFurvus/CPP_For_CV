#pragma once

#ifndef VCFE_DEENCODE_FUNCS_H
#define VCFE_DEENCODE_FUNCS_H

void decode_hex_string(const char* input, char* output);
void encode_hex_string(const char* input, char* output);
void encode_symbol(char ss, char* encodedString);
int count_leading_ones(const char* hexNumber);

#endif // VCFE_DEENCODE_FUNCS_H