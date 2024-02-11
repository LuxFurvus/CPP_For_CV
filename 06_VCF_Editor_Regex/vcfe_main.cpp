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

#include "vcfe_utility_functions.h"
#include "vcfe_printer_functions.h"
////////////////

/*

ORG;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=9A=D0=BE=D0=BC=D0=BF=D0=B0=D0=BD=D0=B8=D1=8F=3B=D0=9E=D1=82=D0=B4=D0=B5=D0=BB

TITLE;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=94=D0=BE=D0=BB=D0=B6=D0=BD=D0=BE=D1=81=D1=82=D1=8C

ORG:Company;Department

TITLE:Position

*/

void print_vcf_org(const std::vector<Addresses>& addresses, std::ofstream& ss) {

	auto bunch_printer = [&](const Addresses& adr, const char* delim, bool to_encode = true) {

		auto no_encode = [](const char* entry) -> char* {
			return const_cast<char*>(entry);
		};

		char* (*encoder)(const char*);
		encoder = (to_encode)? utf8_string_to_hex_string : no_encode;

		ss << encoder(adr.street.c_str()) << delim;
		ss << encoder(adr.city.c_str()) << delim;
		ss << encoder(adr.region.c_str()) << delim;
		ss << encoder(adr.index.c_str()) << delim;
		ss << encoder(adr.country.c_str()) << delim;
	};

	for (auto& adr : addresses) {
		if (adr.is_encoded()) {
			ss << "X-SAMSUNGADR;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;";
			bunch_printer(adr, ";");
			bunch_printer(adr, "=20");
			ss << "\n";
			ss << "ADR;";
			ss << "CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;";
			bunch_printer(adr, ";");
		}
		else {
			ss << "X-SAMSUNGADR;ENCODING=QUOTED-PRINTABLE:;;";
			bunch_printer(adr, ";");
			bunch_printer(adr, "=20");
			ss << "\n";
			ss << "ADR;";
			ss << adr.type << ":;;";
			bunch_printer(adr, ";", false);
		}
		ss << "\n";
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

	make_vcf(cards, "D:\\ForCPP\\VCFE\\TESTITTO.vcf");

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