#define NOUSE
#ifdef NOUSE

#include <string>
#include <vector>
#include <locale>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
extern "C" {
#include "vcfe_encode_to_hex.h"
}
#include "vcfe_data_containers.h"
#include "vcfe_utility_functions.h"
#include "vcfe_printer_functions.h"
////////////////

/*

URL:www.site.com
URL:www.place.org
URL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=92=D0=B5=D0=B1=2D=D1=81=D0=B0=D0=B9=D1=82

NOTE:Write notes here
NOTE;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=97=D0=B0=D0=BC=D0=B5=D1=82=D0=BA=D0=B8=20=D0=B7=D0=B0=D0=BF=D0=B8=

*/

void print_vcf_url(const WorkInfo& work, std::ofstream& ss) {

	if (work.is_empty()) return;

	if (work.is_encoded()) {
		ss << "ORG;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		if (!work.company.empty()) {
			ss << utf8_string_to_hex_string(work.company.c_str());
		}
		ss << "=3B";
		if (!work.department.empty()) {
			ss << utf8_string_to_hex_string(work.department.c_str());
		}
		ss << "\n";
		if (!work.title.empty()) {
			ss << "TITLE;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
			ss << utf8_string_to_hex_string(work.title.c_str());
			ss << "\n";
		}
	}
	else {
		ss << "ORG:";
		if (!work.company.empty()) {
			ss << work.company;
		}
		ss << ";";
		if (!work.department.empty()) {
			ss << work.department;
		}
		ss << "\n";
		if (!work.title.empty()) {
			ss << "TITLE:";
			ss << work.title;
			ss << "\n";
		}
	}
	ss << "\n";
	return;
}

////////////////

void make_vcf(const std::vector<ContactData>& cards, const char* vcf_name) {
	std::ofstream vcf_stream;
	vcf_stream.open(vcf_name, std::ios::trunc);
	if (!vcf_stream.is_open()) {
		printf("Cannot create file %s here!\n", vcf_name);
		return;
	}

	for (const ContactData& card : cards) {
		vcf_stream << "\nBEGIN:VCARD\n";
		vcf_stream << "VERSION:" << card.version << "\n\n";
		//////////////////////////
		print_vcf_name(card.names, vcf_stream);
		print_vcf_phonetics(card.phonetic_name, vcf_stream);
		print_vcf_nickname(card.nickname, vcf_stream);
		print_vcf_telephones(card.tels, vcf_stream);
		print_vcf_email(card.emails, vcf_stream);
		print_vcf_address(card.addresses, vcf_stream);
		print_vcf_company(card.workinfo, vcf_stream);
		//////////////////////////
		vcf_stream << "END:VCARD\n\n";
	}

	vcf_stream.close();
}

////////////////

////////////////

int main() {
	system("chcp 65001 >NUL");
	//setlocale(LC_ALL, "en_US.utf8");

	std::vector<std::string> lines;

	collect_lines(lines, "AAA.vcf");

	std::vector<ContactData> cards;

	analyse_lines(lines, cards);

	//print_cards(cards);

	make_vcf(cards, "D:\\ForCPP\\CPP_For_CV\\06_VCF_Editor_Regex\\TESTITTO.vcf");

	return 0;
}

#endif // NOUSE

//
//
//Создать профиль с текстом на латинице
//Может ли карточка содержать несколько NOTE
//Увеличить количество дат
//
//