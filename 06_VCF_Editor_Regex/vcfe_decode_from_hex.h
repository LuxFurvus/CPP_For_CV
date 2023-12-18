#pragma once
#ifndef VCFE_ENDECODE_FUNC_H
#define VCFE_ENDECODE_FUNC_H

#include <stdint.h>
#include <wchar.h>

/**
 * @brief Convert a hexadecimal string of chars to an array of hexadecimal numbers.
 * @param str       The hexadecimal string of chars to convert.
 * @param hexnums   An array to store the resulting hexadecimal numbers.
 */
void hex_string_to_hexnums(char* str, uint32_t* hexnums);

/**
 * @brief Convert an array of hexadecimal numbers to a wide character.
 * @param hex      The array of hexadecimal numbers.
 * @param bit_size The size of each hexadecimal number in bits.
 * @return         The resulting wide character.
 */
wchar_t hexnums_to_wchar(const uint32_t hex[3], const int bit_size);

/**
 * @brief Convert a hexadecimal string to a UTF-8 wide character string.
 * @param hexString The hexadecimal string to convert.
 * @param utf8_str  A buffer to store the resulting UTF-8 wide character string.
 * @return          The resulting wide character string.
 */
wchar_t* hex_string_to_utf8_string(const char* hexString);

//////////////////////////////////////////
//#define VCF_DEBUG_ONLY
#ifdef VCF_DEBUG_ONLY

/**
 * @brief Test function for hex_string_to_utf8_string.
 */
void hex_string_to_utf8_string_TEST(void);

#endif // VCF_DEBUG_ONLY

#endif // VCFE_ENDECODE_FUNC_H
