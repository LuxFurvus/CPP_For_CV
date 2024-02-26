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



*/

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
		print_vcf_event(card.events, vcf_stream);
		print_vcf_socials(card.socials, vcf_stream);
		print_vcf_relations(card.relations, vcf_stream);
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