#pragma once
#ifndef VCFE_PRINTER_FUNCTIONS_H
#define VCFE_PRINTER_FUNCTIONS_H

#include <fstream>
#include "vcfe_regex_parsers.h"

/**
 * @brief Prints a formatted VCF (Virtual Card File) name to the specified output stream.
 *
 * This function takes a 'Names' object and an output file stream ('std::ofstream') and prints
 * a formatted VCF name to the output stream. The format depends on whether the 'Names' object
 * is encoded or not.
 *
 * If the 'Names' object is encoded, the function prints the name in a specific format with
 * UTF-8 encoding. If not encoded, it prints the name as is.
 *
 * @param names The 'Names' object containing the name components.
 * @param ss The output file stream where the formatted name will be printed.
 */
void print_vcf_name(const Names& names, std::ofstream& ss);

void print_vcf_phonetics(const PhoneticName& phonetics, std::ofstream& ss);
void print_vcf_nickname(const NickName& nickname, std::ofstream& ss);
void print_vcf_telephones(const std::vector<Telephones>& tels, std::ofstream& ss);
void print_vcf_email(const std::vector<Emails>& emails, std::ofstream& ss);
void print_vcf_address(const std::vector<Addresses>& addresses, std::ofstream& ss);
void print_vcf_company(const WorkInfo& work, std::ofstream& ss);


#endif // VCFE_PRINTER_FUNCTIONS_H
