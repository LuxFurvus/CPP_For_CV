#pragma once
#ifndef _VCFE_UTILITY_FUNCTIONS_H_
#define _VCFE_UTILITY_FUNCTIONS_H_

#include <vector>
#include <string>
#include <fstream>
#include "vcfe_regex_parsers.h"

/**
 * @brief Collects lines of text from user input.
 *
 * This function collects lines of text from the VCF file
 * and stores them in the provided vector.
 *
 * @param lines = A vector to store the collected lines of text.
 */
void collect_lines(std::vector<std::string>& lines, const std::string& vcf_name);

/**
 * @brief Analyzes lines of text to extract virtual card information and populates a dataset.
 *
 * This function parses the input lines using regular expressions
 * and extracts information to populate a vector of ContactData objects,
 * representing virtual cards.
 *
 * @param lines A vector of strings containing lines of text to be analyzed.
 * @param dataset A vector to store the extracted ContactData objects.
 */
void analyse_lines(std::vector<std::string>& lines, std::vector<ContactData>& dataset);

/**
 * @brief Prints the information of virtual cards in the console.
 *
 * This function takes a vector of ContactData objects representing
 * virtual cards and prints their information to the console.
 *
 * @param cards A vector containing ContactData objects representing virtual cards.
 */
void print_cards(const std::vector<ContactData>& cards);

#endif // _VCFE_UTILITY_FUNCTIONS_H_
