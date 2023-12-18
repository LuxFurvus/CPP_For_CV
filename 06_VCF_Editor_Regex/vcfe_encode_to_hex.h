#pragma once
#ifndef _VCFE_DECODE_FROM_HEX_H_
#define _VCFE_DECODE_FROM_HEX_H_

#include <wchar.h>
#include <stdint.h>

/**
 * @brief Find the index of the first zero in an array.
 * @param array      The array to search.
 * @param array_size The size of the array.
 * @return The index of the first zero, or -1 if not found.
 */
int find_first_zero(int32_t* array, int array_size);

/**
 * @brief Convert a UTF-8 character to an array of hexadecimal numbers.
 * @param utf8_char The UTF-8 character to convert.
 * @param hexnums   An array to store the resulting hexadecimal numbers.
 * @param hexnums_size The size of the hexnums array.
 * @return The number of hexadecimal numbers written to hexnums + 1.
 */
int wchar_to_hexnums(wchar_t utf8_char, int32_t* hexnums, int hexnums_size);

/**
 * @brief Convert a UTF-8 string to an array of hexadecimal numbers.
 * @param hex_string The UTF-8 string to convert.
 * @param hex_end    Pointer to an integer to store the index of the first non-hex character.
 * @return An array of hexadecimal numbers.
 */
int32_t* utf8_to_hexnums(const wchar_t* hex_string, int* hex_end);

/**
 * @brief Convert an array of hexadecimal numbers to a string of chars.
 * @param utf8_string The array of hexadecimal numbers representing a UTF-8 string.
 * @return A string of chars.
 */
char* utf8_string_to_hex_string(const wchar_t* utf8_string);

//#define VCF_DEBUG_ONLY
#ifdef VCFE_DEBUG_ONLY

/**
 * @brief Test function for utf8_to_hexnums.
 * @param hex_string The UTF-8 string to test.
 */
void utf8_to_hexnums_TEST(const wchar_t* hex_string);

/**
 * @brief Test function for utf8_string_to_hex_string.
 */
void utf8_string_to_hex_string_TEST(void);

#endif // VCFE_DEBUG_ONLY

#endif // _VCFE_DECODE_FROM_HEX_H_
