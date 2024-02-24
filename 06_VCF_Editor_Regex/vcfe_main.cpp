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

X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;--01-12;1;;;;;;;;;;;;;0
X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;2024-01-12;0;SpecDate;;;;;;;;;;;;0
X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;--04-12;2;;;;;;;;;;;;;0

X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;--06-01;1;;;;;;;;;;;;;0
X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;2022-01-01;2;;;;;;;;;;;;;0
X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/contact_event;=2D=2D=30=36=2D=31=35;=30;=D0=92=D0=B0=D0=B6=D0=BD=D0=B0=D1=8F=20=D0=94=D0=B0=D1=82=D0=B0=23=E2=
=82=BD;;;;;;;;;;;;=30

BDAY:2000-01-01

*/

void print_vcf_event(const std::vector<Event>& events, std::ofstream& ss) {
	if (events.empty()) return;

	for (const auto& event : events) {
		if (event.is_encoded()) {
			ss << "X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
			ss << "vnd.android.cursor.item/contact_event;";
			//ss << utf8_string_to_hex_string(note.note_text.c_str());
		}
		else {
			ss << "X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;";
			ss << (event.year.empty()? "-" : event.year);
			ss << "-" << event.month << "-" << event.day << event.event_name;

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
		print_vcf_company(card.workinfo, vcf_stream);
		print_vcf_url(card.urls, vcf_stream);
		print_vcf_note(card.note, vcf_stream);
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