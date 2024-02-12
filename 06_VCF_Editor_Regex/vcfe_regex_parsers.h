#pragma once
#ifndef VCFE_REGEX_PARSERS_H
#define VCFE_REGEX_PARSERS_H

#include <string>
#include <memory>
#include "vcfe_data_containers.h"


void version_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void name_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void tel_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void email_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void phonetics_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void nick_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void address_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void company_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void title_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void url_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void note_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void events_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
bool is_socials_line(const std::string& line, std::string& sns_name);
void socials_parser(const std::string& line, std::unique_ptr<ContactData>& current_card, const std::string& sns_name);
void relations_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);

#endif // !VCFE_REGEX_PARSERS_H
