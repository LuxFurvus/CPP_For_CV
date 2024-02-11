#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include "vcfe_regex_parsers.h"

void collect_lines(std::vector<std::string>& lines, const std::string& vcf_name) {
	std::ifstream file(vcf_name, std::ios::in);
	std::vector<std::string> newlines;

	if (!file.is_open()) {
		printf("Can't open the file!\n");
		return;
	}

	std::string oneline;

	while (std::getline(file, oneline)) {
		if (oneline.empty()) {
			continue;
		}

		if (oneline[0] == '=' || oneline[0] == ';') {
			//oneline.erase(oneline.begin());
			auto& current_string = lines[lines.size() - 1];
			current_string.erase(current_string.end() - 1);
			current_string.append(oneline);
			continue;
		}

		lines.push_back(oneline);
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void print_cards(const std::vector<ContactData>& cards) {
	for (auto& card : cards) {
		printf("\n+++NEW CARD+++\n");

		if (!card.version.empty()) {
			std::cout << "VERSION: " << card.version << '\n';
		}

		if (!card.names.is_empty()) {
			printf("NAME:\n\t");
			for (int i = 0; i < 5; ++i) {
				std::cout << card.names[i] << ' ';
			}
			printf("\n");
		}

		if (!card.phonetic_name.is_empty()) {
			std::cout
				<< "PHONETIC NAME:\n\t"
				<< card.phonetic_name.first << " "
				<< card.phonetic_name.middle << " "
				<< card.phonetic_name.last << "\n";
		}

		if (!card.nickname.nick.empty()) {
			printf("NICKNAME:\n\t");
			std::cout << card.nickname.nick;
			printf("\n");
		}

		if (!card.tels.empty()) {
			printf("TELEPHONE NUMBERS:\n");
			for (auto& tel : card.tels) {
				std::cout << '\t' << tel.type << ": " << tel.number << "\n";
			}
		}

		if (!card.emails.empty()) {
			printf("E-MAILS:\n");
			for (auto& email : card.emails) {
				std::cout << '\t' << email.type << ": " << email.address << "\n";
			}
		}

		if (!card.addresses.empty()) {
			printf("ADDRESSES:\n");
			for (const auto& scric : card.addresses) {
				std::cout << '\t' << scric[0] << ": ";
				for (int i = 1; i < 6; ++i) {
					std::cout << scric[i] << " ";
				}
				printf("\n");
			}
		}

		if (!card.workinfo.is_empty()) {
			printf("ORGANISATION:");
			std::cout
				<< "\n\tCompany: " << card.workinfo.company
				<< "\n\tDepartment: " << card.workinfo.department
				<< "\n\tTitle: " << card.workinfo.title;
			printf("\n");
		}

		if (!card.urls.empty()) {
			printf("URLs:\n");
			for (const auto& url : card.urls) {
				std::cout << "\t" << url << "\n";
			}
		}
		if (!card.note.empty()) {
			std::cout << "NOTES:\n\t" << card.note << "\n";
		}

		if (!card.events.empty()) {
			printf("DATES:\n");
			for (const auto& event : card.events) {
				std::cout << "\t" << event.event_name << ": "
					<< event.day << '-'
					<< event.month << '-'
					<< event.year << '\n';
			}
		}

		if (!card.socials.empty()) {
			printf("SOCIAL NETWORKS:\n");
			for (const auto& sns : card.socials) {
				std::cout << "\t"
					<< sns.name << ": "
					<< sns.contact << '\n';
			}
		}

		if (!card.relations.empty()) {
			printf("RELATIONS:\n");
			for (const auto& rel : card.relations) {
				std::cout << "\t"
					<< rel.type_name << " ("
					<< rel.type_num << "):\t\t"
					<< rel.name << '\n';
			}
			printf("\n");
		}
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void analyse_lines(std::vector<std::string>& lines, std::vector<ContactData>& dataset) {
	std::unique_ptr<ContactData> current_card;

	for (auto& line : lines) {
		if (line.contains("BEGIN:VCARD")) {
			std::unique_ptr<ContactData> card(new ContactData);
			current_card = std::move(card);
			continue;
		}
		if (line.contains("VERSION:")) {
			version_parser(line, current_card);
			continue;
		}
		////////////////////////////////////
		if (line[0] == 'N' && (line[1] == ';' || line[1] == ':')) {
			name_parser(line, current_card);
			continue;
		}
		if (line.contains("X-PHONETIC-")) {
			phonetics_parser(line, current_card);
			continue;
		}
		if (line.contains("TEL;")) {
			tel_parser(line, current_card);
			continue;
		}
		if (line.contains("vnd.android.cursor.item/nickname")) {
			nick_parser(line, current_card);
			continue;
		}
		if (line.contains("EMAIL;") || line.contains("EMAIL:")) {
			email_parser(line, current_card);
			continue;
		}
		if (line.contains("ADR;") && !line.contains("SAMSUNGADR;")) {
			address_parser(line, current_card);
			continue;
		}
		if (line.contains("ORG;")) {
			company_parser(line, current_card);
			continue;
		}
		if (line.contains("TITLE;")) {
			title_parser(line, current_card);
			continue;
		}
		if (line.contains("URL;") || line.contains("URL:")) {
			url_parser(line, current_card);
			continue;
		}
		if (line.contains("NOTE;") || line.contains("NOTE:")) {
			note_parser(line, current_card);
			continue;
		}
		if (line.contains("vnd.android.cursor.item/contact_event;") || line.contains("BDAY:")) {
			events_parser(line, current_card);
			continue;
		}
		std::string sns_name;
		if (line[0] == 'X' && is_socials_line(line, sns_name)) {
			socials_parser(line, current_card, sns_name);
			continue;
		}
		if (line.contains("vnd.android.cursor.item/relation")) {
			relations_parser(line, current_card);
			continue;
		}
		////////////////////////////////////
		if (line.contains("END:VCARD")) {
			dataset.push_back(std::move(*current_card));
			continue;
		}
	}
}