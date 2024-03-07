#include <string>
#include <fstream>
#include <iostream>
#include <functional>

extern "C" {
#include "vcfe_encode_to_hex.h"
}
#include "vcfe_vcf_printer.hpp"

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

bool VcfPrinter::check_for_non_ascii(
	const std::initializer_list<std::reference_wrapper<const std::string>>& strings) const {
	for (const auto& str : strings) {
		for (unsigned char ch : str.get()) {
			if (ch > 127) {
				return true; // Non-ASCII character found
			}
		}
	}
	return false;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_name(const NameRecord& names, std::ofstream& ss) const {
	bool has_non_ascii = check_for_non_ascii({
		names.family,names.personal,
		names.father,names.address_form,
		names.suffix });

	if (has_non_ascii) {
		ss << "N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		ss << utf8_string_to_hex_string(names.family.c_str()) << ';';
		ss << utf8_string_to_hex_string(names.personal.c_str()) << ';';
		ss << utf8_string_to_hex_string(names.father.c_str()) << ';';
		ss << utf8_string_to_hex_string(names.address_form.c_str()) << ';';
		ss << utf8_string_to_hex_string(names.suffix.c_str());
		ss << '\n';

		ss << "FN;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		ss << utf8_string_to_hex_string(names.address_form.c_str()) << "=20";
		ss << utf8_string_to_hex_string(names.personal.c_str()) << "=20";
		ss << utf8_string_to_hex_string(names.father.c_str()) << "=20";
		ss << utf8_string_to_hex_string(names.family.c_str()) << "=2C=20";
		ss << utf8_string_to_hex_string(names.suffix.c_str());
		ss << '\n';
	}
	else {
		ss << "N:";
		ss << names.family << ';';
		ss << names.personal << ';';
		ss << names.father << ';';
		ss << names.address_form << ';';
		ss << names.suffix;
		ss << '\n';

		ss << "FN:";
		ss << names.address_form << " ";
		ss << names.personal << " ";
		ss << names.father << " ";
		ss << names.family << ", ";
		ss << names.suffix;
		ss << '\n';
	}
	ss << '\n';
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_phonetics(
	const PhoneticRecord& phonetics, 
	std::ofstream& ss) const 
{
	bool has_non_ascii = check_for_non_ascii({
		phonetics.first,
		phonetics.middle,
		phonetics.last });

	if (has_non_ascii) {
		ss << "X-PHONETIC-FIRST-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		ss << utf8_string_to_hex_string(phonetics.first.c_str()) << "\n";
		ss << "X-PHONETIC-MIDDLE-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		ss << utf8_string_to_hex_string(phonetics.middle.c_str()) << "\n";
		ss << "X-PHONETIC-LAST-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		ss << utf8_string_to_hex_string(phonetics.last.c_str()) << "\n";
	}
	else {
		ss << "X-PHONETIC-FIRST-NAME:";
		ss << phonetics.first.c_str() << "\n";
		ss << "X-PHONETIC-MIDDLE-NAME:";
		ss << phonetics.middle.c_str() << "\n";
		ss << "X-PHONETIC-LAST-NAME:";
		ss << phonetics.last.c_str() << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_nickname(
	const NickNameRecord& nickname, 
	std::ofstream& ss) const 
{
	// Directly check if nickname.nick contains non-ASCII characters
	bool has_non_ascii = check_for_non_ascii({ nickname.nick });

	if (has_non_ascii) {
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

void VcfPrinter::print_vcf_telephones(
	const std::vector<TelephoneRecord>& telephones, 
	std::ofstream& ss) const 
{
	auto is_custom_tel = [&](const std::string& tel_type) {
		for (const auto& tel : telephone_types) {
			if (tel_type == tel) {
				return false;
			}
		}
		return true;		
		};

	for (auto& tel : telephones) {
		bool has_non_ascii = check_for_non_ascii({ tel.type });

		//TEL;X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,=D0=9E
		if (has_non_ascii) {
			ss << "TEL;X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,";
			ss << utf8_string_to_hex_string(tel.type.c_str());
			ss << "):";
		}
		//TEL;VOICE:77777
		//TEL;X-CustomisoNum:66666
		else {
			ss << "TEL;";
			if (is_custom_tel(tel.type)) ss << "X-";
			ss << tel.type << ":";
		}
		ss << tel.number.c_str();
		ss << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_email(
	const std::vector<EmailRecord>& emails, 
	std::ofstream& ss) const 
{
	for (auto& em : emails) {
		bool has_non_ascii_type = check_for_non_ascii({ em.type });
		bool has_non_ascii_address = check_for_non_ascii({ em.address });

		//EMAIL:email1
		//EMAIL;WORK:workmail
		if (!has_non_ascii_address && !has_non_ascii_type) {
			ss << "EMAIL";
			if (em.type.empty()) ss << ":";
			else ss << ";" << em.type << ":";
			ss << em.address;
		}
		//EMAIL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=B4
		else if (has_non_ascii_address && em.type.empty()) {
			ss << "EMAIL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
			ss << utf8_string_to_hex_string(em.address.c_str());
		}
		//EMAIL;X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,=D0=9E);CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=BF
		else if (has_non_ascii_address && has_non_ascii_type) {
			ss << "EMAIL;X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,";
			ss << utf8_string_to_hex_string(em.type.c_str());
			ss << ");CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
			ss << utf8_string_to_hex_string(em.address.c_str());
		}
		//EMAIL;HOME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=AD
		else if (has_non_ascii_address && !has_non_ascii_type) {
			ss << "EMAIL;";
			ss << em.type;
			ss << ";CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
			ss << utf8_string_to_hex_string(em.address.c_str());
		}
		ss << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_address(
	const std::vector<AddressRecord>& addresses, 
	std::ofstream& ss) const 
{
	auto no_encode = [](const char* entry) -> char* {
		return const_cast<char*>(entry);
		};

	auto regular_printer = [&](const AddressRecord& adr, const char* delim, bool to_encode = true) {
		char* (*encoder)(const char*);
		encoder = (to_encode) ? utf8_string_to_hex_string : no_encode;

		ss << encoder(adr.street.c_str()) << delim;
		ss << encoder(adr.city.c_str()) << delim;
		ss << encoder(adr.region.c_str()) << delim;
		ss << encoder(adr.index.c_str()) << delim;
		ss << encoder(adr.country.c_str());
		};

	auto special_printer = [&](const AddressRecord& adr) {
		ss << utf8_string_to_hex_string(adr.street.c_str()) << "=0A";
		ss << utf8_string_to_hex_string(adr.city.c_str()) << "=2C=20";
		ss << utf8_string_to_hex_string(adr.region.c_str()) << "=20";
		ss << utf8_string_to_hex_string(adr.index.c_str()) << "=0A";
		ss << utf8_string_to_hex_string(adr.country.c_str());
		};

	for (auto& adr : addresses) {
		bool has_non_ascii = check_for_non_ascii({
			adr.street, adr.city,
			adr.region, adr.index, adr.country
			});

		if (has_non_ascii) {
			//X-SAMSUNGADR;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;=D0=A3
			ss << "X-SAMSUNGADR;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;";
			regular_printer(adr, ";");
			ss << ";";
			special_printer(adr);
			ss << "\n";
			//ADR;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;=D0=A3
			ss << "ADR;";
			if (adr.type.empty() == false) ss << adr.type << ";";
			ss << "CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;";
			regular_printer(adr, ";");
		}
		else {
			ss << "X-SAMSUNGADR;ENCODING=QUOTED-PRINTABLE:;;";
			regular_printer(adr, ";");
			ss << ";";
			special_printer(adr);
			ss << "\n";
			//ADR;WORK:;;Street;City;Region;Index;Country
			ss << "ADR;";
			ss << adr.type << ":;;";
			regular_printer(adr, ";", false);
		}
		ss << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_company(
	const WorkInfoRecord& work, 
	std::ofstream& ss) const 
{
	if (work.is_empty()) return;

	bool orgnn_has_non_ascii;

	if (work.company.empty() && work.department.empty()) {
		goto title_pos;
	}
	orgnn_has_non_ascii = check_for_non_ascii({ work.company, work.department });

	if (orgnn_has_non_ascii) {
		//ORG;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=9A
		ss << "ORG;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		if (!work.company.empty()) {
			ss << utf8_string_to_hex_string(work.company.c_str());
		}
		ss << "=3B";
		if (!work.department.empty()) {
			ss << utf8_string_to_hex_string(work.department.c_str());
		}
		ss << "\n";
	}
	else {
		//ORG:Company;Department
		ss << "ORG:";
		if (!work.company.empty()) {
			ss << work.company;
		}
		ss << ";";
		if (!work.department.empty()) {
			ss << work.department;
		}
		ss << "\n";
	}

title_pos:

	if (work.title.empty()) {
		ss << "\n";
		return;
	}
	bool title_has_non_ascii = check_for_non_ascii({ work.title });

	if (title_has_non_ascii) {
		//TITLE;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=94
		ss << "TITLE;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		ss << utf8_string_to_hex_string(work.title.c_str());
		ss << "\n";
	}
	else {
		//	TITLE:Position
		ss << "TITLE:";
		ss << work.title;
		ss << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_url(
	const std::vector<UrlRecord>& urls, 
	std::ofstream& ss) const
{
	if (urls.empty()) return;

	for (auto& url : urls) {
		bool has_non_ascii = check_for_non_ascii({ url.url_address });

		if (has_non_ascii) {
			ss << "URL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
			ss << utf8_string_to_hex_string(url.url_address.c_str());
		}
		else {
			ss << "URL:";
			ss << url.url_address;
		}
		ss << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_note(
	const NoteRecord& note, 
	std::ofstream& ss) const 
{
	if (note.note_text.empty()) return;

	bool has_non_ascii = check_for_non_ascii({ note.note_text });

	if (has_non_ascii) {
		ss << "NOTE;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
		ss << utf8_string_to_hex_string(note.note_text.c_str());
	}
	else {
		ss << "NOTE:";
		ss << note.note_text;
	}

	ss << "\n\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_event(
	const std::vector<EventRecord>& events, 
	std::ofstream& ss) const 
{
	if (events.empty()) return;

	bool has_non_ascii;
	auto print_date = [&](const EventRecord& ee) {
		ss << (ee.year.empty() ? "-" : ee.year) << "-";
		ss << ee.month << "-";
		ss << ee.day;
		};

	for (const auto& event : events) {
		switch (event.event_type) {
		case EventType::SPECIAL:

			has_non_ascii = check_for_non_ascii({ event.event_name });

			if (has_non_ascii) {
				//X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/contact_event;=2D=2D=30=36=2D=31=35;=30;=D0=92=D0=B0=D0=B6=D0=BD=D0=B0=D1=8F=20=D0=94=D0=B0=D1=82=D0=B0=23=E2=82=BD;;;;;;;;;;;;=30
				ss << "X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
				ss << "vnd.android.cursor.item/contact_event;";

				ss << (event.year.empty() ? "=2D" :
					utf8_string_to_hex_string(event.year.c_str())) << "=2D";
				ss << (event.month.empty() ? "=2D" :
					utf8_string_to_hex_string(event.month.c_str())) << "=2D";
				ss << (event.day.empty() ? "=2D" :
					utf8_string_to_hex_string(event.day.c_str()));

				ss << ";=30;";
				ss << utf8_string_to_hex_string(event.event_name.c_str());
				ss << ";;;;;;;;;;;;=30";
			}
			else {
				//X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;2024-01-12;0;SpecDate;;;;;;;;;;;;0
				ss << "X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;";
				print_date(event);
				ss << ";0;";
				ss << event.event_name;
				ss << ";;;;;;;;;;;;0";
			}
			break;
		case EventType::ANNIVERSARY:
			// X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;--01-12;1;;;;;;;;;;;;;0
			ss << "X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;";
			print_date(event);
			ss << ";1;";
			ss << ";;;;;;;;;;;;0";
			break;
		case EventType::OTHER:
			//X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;--04-12;2;;;;;;;;;;;;;0
			//X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;2022-01-01;2;;;;;;;;;;;;;0
			ss << "X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;";
			print_date(event);
			ss << ";2;";
			ss << ";;;;;;;;;;;;0";
			break;
		case EventType::BDAY:
			//BDAY:2000-01-01
			ss << "BDAY:";
			print_date(event);
			break;
		case EventType::NONE:
		default: break;
		}
		ss << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_socials(
	const std::vector<SocialNetRecord>& socials, 
	std::ofstream& ss) const
{
	if (socials.empty()) return;

	auto is_regular_sns_name = [&](const std::string& sns_name) {
		for (const auto& sns : socials_names) {
			if (sns_name == sns) {
				return true;
			}
		}
		return false;
		};


	for (const auto& sn : socials) {
		bool has_non_ascii = check_for_non_ascii({ sn.name, sn.contact });

		std::string sns_name;
		if (sn.name == "SKYPE") {
			sns_name = "SKYPE-USERNAME";
		}
		else if (sn.name == "HANGOUTS") {
			sns_name = "GOOGLE-TALK";
		}
		else {
			sns_name = sn.name;
		}

		bool is_regular_sns = is_regular_sns_name(sns_name);

		if (has_non_ascii) {
			if (is_regular_sns) {
			    //X-JABBER;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=90=D0=B4
				ss << "X-" << sns_name;
			}
			else {
			    //X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,=D0=9E);CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=9A
				ss << "X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,";
				ss << utf8_string_to_hex_string(sns_name.c_str()) << ")";
			}
			ss << ";CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
			ss << utf8_string_to_hex_string(sn.contact.c_str());
		}
		else {
			if (is_regular_sns) {
			    //X-WHATSAPP:whazzzzap
				ss << "X-" << sns_name << ":" << sn.contact;
			}
			else {
			    //X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,Special Sns):this sns
				ss << "X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,";
				ss << sns_name << "):" << sn.contact;
			}
		}
		ss << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_vcf_relations(
	const std::vector<RelationRecord>& relations, 
	std::ofstream& ss) const 
{
	if (relations.empty()) return;

	for (const auto& rel : relations) {

		bool has_non_ascii = check_for_non_ascii({ rel.name, rel.type_name });

		if (has_non_ascii) {
			// X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/relation;=D0=9F;=31;;;;;;;;;;;;;
			// X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/relation;=D0=A1;=30;=D0=A1;;;;;;;;;;;;
			ss << "X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:";
			ss << "vnd.android.cursor.item/relation;";
			ss << utf8_string_to_hex_string(rel.name.c_str()) << ";";
			ss << utf8_string_to_hex_string(std::to_string(rel.type_num).c_str()) << ";";
			if (rel.type_num == 0) ss << utf8_string_to_hex_string(rel.type_name.c_str());
		}
		else {
			// X-ANDROID-CUSTOM:vnd.android.cursor.item/relation;Assistant;1;;;;;;;;;;;;;
			// X-ANDROID-CUSTOM:vnd.android.cursor.item/relation;Some Guy;0;Other Relative;;;;;;;;;;;;
			ss << "X-ANDROID-CUSTOM:vnd.android.cursor.item/relation;";
			ss << rel.name << ";";
			ss << rel.type_num << ";";
			if (rel.type_num == 0) ss << rel.type_name;
		}
		ss << ";;;;;;;;;;;;";
		ss << "\n";
	}
	ss << "\n";
	return;
}

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_to_console(
	const std::vector<ContactData>& cards) const 
{
	for (auto& card : cards) {
		printf("\n+++NEW CARD+++\n");

		if (!card.version.empty()) {
			std::cout << "VERSION: " << card.version << '\n';
		}

		if (!card.names.is_empty()) {
			printf("NAME:\n\t");
			std::cout << card.names.family << ' ';
			std::cout << card.names.personal << ' ';
			std::cout << card.names.father << ' ';
			std::cout << card.names.address_form << ' ';
			std::cout << card.names.suffix << ' ';
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

		if (!card.telephones.empty()) {
			printf("TELEPHONE NUMBERS:\n");
			for (auto& tel : card.telephones) {
				std::cout << '\t' << tel.type << ": " << tel.number << "\n";
			}
		}

		if (!card.emails.empty()) {
			printf("E-MAILS:\n");
			for (auto& email : card.emails) {
				std::string email_type{ (email.type.empty()) ? "OTHER" : email.type };
				std::cout << '\t' << email_type << ": " << email.address << "\n";
			}
		}

		if (!card.addresses.empty()) {
			printf("ADDRESSES:\n");
			for (const auto& scric : card.addresses) {
				std::string scric_type{ (scric.type.empty()) ? "OTHER" : scric.type };
				std::cout << '\t' << scric_type << ": ";
				std::cout << scric.street << " ";
				std::cout << scric.city << " ";
				std::cout << scric.region << " ";
				std::cout << scric.index << " ";
				std::cout << scric.country << " ";
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
				std::cout << "\t" << url.url_address << "\n";
			}
		}
		if (!card.note.note_text.empty()) {
			std::cout << "NOTES:\n\t" << card.note.note_text << "\n";
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

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++

void VcfPrinter::print_to_file(
	const std::vector<ContactData>& cards, 
	const std::string& filename) const
{
	std::ofstream vcf_stream;
	vcf_stream.open(std::string(filename).append(".vcf"), std::ios::trunc);
	if (!vcf_stream.is_open()) {
		printf("Cannot create file %s here!\n", filename.c_str());
		return;
	}

	for (const ContactData& card : cards) {
		vcf_stream << "\nBEGIN:VCARD\n";
		vcf_stream << "VERSION:" << card.version << "\n\n";
		//////////////////////////
		print_vcf_name(card.names, vcf_stream);
		print_vcf_phonetics(card.phonetic_name, vcf_stream);
		print_vcf_nickname(card.nickname, vcf_stream);
		print_vcf_telephones(card.telephones, vcf_stream);
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

//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++//++