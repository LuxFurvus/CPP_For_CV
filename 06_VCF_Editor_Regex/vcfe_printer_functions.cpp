extern "C" {
#include "vcfe_encode_to_hex.h"
}

#include <string>
#include <fstream>
#include "vcfe_regex_parsers.h"

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void print_vcf_name(const Names& names, std::ofstream& ss) {

	if (names.is_encoded()) {		
		ss << "N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		
		for (int i = 0; i < 4; i++) {
			ss << utf8_string_to_hex_string(names[i].c_str()) << ';';
		}
		ss << utf8_string_to_hex_string(names[4].c_str()) << '\n';

		ss << "FN;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		for (int i = 0; i < 5; i++) {
			ss << utf8_string_to_hex_string(names[i].c_str()) << "=20";
		}
		ss << '\n';
	}
	else {
		ss << "N:";
		for (int i = 0; i < 4; i++) {
			ss << names[i].c_str() << ';';
		}
		ss << names[4].c_str() << '\n';

		ss << "FN:";
		for (int i = 0; i < 5; i++) {
			ss << names[i].c_str() << ' ';
		}
		ss << '\n';
	}
	ss << '\n';
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void print_vcf_phonetics(const PhoneticName& phonetics, std::ofstream& ss) {

	if (phonetics.is_encoded()) {
		ss << "X-PHONETIC-FIRST-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		ss << utf8_string_to_hex_string(phonetics.first.c_str()) << ";\n";
		ss << "X-PHONETIC-MIDDLE-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		ss << utf8_string_to_hex_string(phonetics.middle.c_str()) << ";\n";
		ss << "X-PHONETIC-LAST-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		ss << utf8_string_to_hex_string(phonetics.last.c_str()) << ";\n";
	}
	else {
		ss << "X-PHONETIC-FIRST-NAME:";
		ss << phonetics.first.c_str() << ";\n";
		ss << "X-PHONETIC-MIDDLE-NAME:";
		ss << phonetics.middle.c_str() << ";\n";
		ss << "X-PHONETIC-LAST-NAME:";
		ss << phonetics.last.c_str() << ";\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void print_vcf_nickname(const NickName& nickname, std::ofstream& ss) {

	if (nickname.is_encoded()) {
		ss << "X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/nickname;";
		ss << utf8_string_to_hex_string(nickname.nick.c_str()) << ";;;;;;;;;;;;;;\n";
	}
	else {
		ss << "X-ANDROID-CUSTOM:vnd.android.cursor.item/nickname;";
		ss << nickname.nick.c_str() << ";;;;;;;;;;;;;;\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void print_vcf_telephones(const std::vector<Telephones>& tels, std::ofstream& ss) {
	for (auto& tel : tels) {
		if (tel.is_encoded()) {
			ss << "TEL;X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,";
			ss << utf8_string_to_hex_string(tel.type.c_str());
			ss << "):";
		}
		else {
			ss << "TEL;";
			ss << tel.type << ":";
		}
		ss << tel.number.c_str();
		ss << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void print_vcf_email(const std::vector<Emails>& emails, std::ofstream& ss) {
	for (auto& em : emails) {
		if (em.is_encoded()) {
			ss << "EMAIL;X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,";
			ss << utf8_string_to_hex_string(em.type.c_str());
			ss << ");CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
			ss << utf8_string_to_hex_string(em.address.c_str());
		}
		else {
			ss << "EMAIL;";
			ss << em.type << ":";
			ss << em.address;
		}
		ss << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void print_vcf_address(const std::vector<Addresses>& addresses, std::ofstream& ss) {

	auto bunch_printer = [&](const Addresses& adr, const char* delim, bool to_encode = true) {

		auto no_encode = [](const char* entry) -> char* {
			return const_cast<char*>(entry);
			};

		char* (*encoder)(const char*);
		encoder = (to_encode) ? utf8_string_to_hex_string : no_encode;

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
