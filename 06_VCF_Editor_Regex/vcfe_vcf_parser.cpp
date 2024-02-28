#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <regex>

extern "C" {
#include "vcfe_decode_from_hex.h"
}
#include "vcfe_vcf_parser.h"

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

std::string VcfParser::decode(const char* str) const {
	char* utf8_string = hex_string_to_utf8_string(str);
	std::string decoded_string(utf8_string);
	free(utf8_string);

	return decoded_string;
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::collect_lines(const std::string& vcf_name) const {

	std::ifstream file(vcf_name, std::ios::in);

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

void VcfParser::parse_lines(std::vector<ContactData>& dataset) {

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
		if (line.contains("ORG;") || line.contains("ORG:")) {
			company_parser(line, current_card);
			continue;
		}
		if (line.contains("TITLE;") || line.contains("TITLE:")) {
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
			dataset.emplace_back(std::move(*current_card));
			continue;
		}
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::version_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	//VERSION:2.1
	std::regex version_pattern("^VERSION:([^:]*)$");

	if (std::regex_search(line, mm, version_pattern)) {
		current_card->version = mm[1].str();
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::name_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	// Regex pattern for extracting names
	std::regex pattern1("^N[^:]*:(?:[^:]*:)*([^;]*);([^;]*);([^;]*);([^;]*);([^;]*)$");

	// Check if line needs to be decoded
	bool is_to_decode = (line.contains("ENCODING=QUOTED-PRINTABLE"));
	if (is_to_decode) {
		current_card->names.set_encoded_state();
	}

	if (std::regex_search(line, mm, pattern1)) {
		// Extract names from the matched parts
		for (int i = 1; i < mm.size(); ++i) {
			if (!mm[i].str().empty()) continue;
			// Decode if needed
			std::string decoded_name = (is_to_decode) ? decode(mm[i].str().c_str()) : mm[i].str();

			// Assign to appropriate member variable
			switch (i) {
			case 1:
				current_card->names.family = decoded_name;
				break;
			case 2:
				current_card->names.personal = decoded_name;
				break;
			case 3:
				current_card->names.father = decoded_name;
				break;
			case 4:
				current_card->names.address_form = decoded_name;
				break;
			case 5:
				current_card->names.suffix = decoded_name;
				break;
			default:
				break;
			}
		}
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::phonetics_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	bool is_to_decode{ false };
	if (line.contains("ENCODING=QUOTED-PRINTABLE")) {
		is_to_decode = true;
		current_card->phonetic_name.set_encoded_state();
	}

	///////////////////////////////////
//X-PHONETIC-FIRST-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0
//X-PHONETIC-MIDDLE-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0
//X-PHONETIC-LAST-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0

	std::regex phonetic_pattern("^X-PHONETIC-[^:]*:([^;]*)$");

	if (std::regex_search(line, mm, phonetic_pattern)) {
		if (line.contains("X-PHONETIC-FIRST-NAME")) {
			if (is_to_decode) {
				current_card->phonetic_name.first = decode(mm[1].str().c_str());
			}
			else {
				current_card->phonetic_name.first = mm[1];
			}
		}
		else if (line.contains("X-PHONETIC-MIDDLE-NAME")) {
			if (is_to_decode) {
				current_card->phonetic_name.middle = decode(mm[1].str().c_str());
			}
			else {
				current_card->phonetic_name.middle = mm[1];
			}
		}
		else if (line.contains("X-PHONETIC-LAST-NAME")) {
			if (is_to_decode) {
				current_card->phonetic_name.last = decode(mm[1].str().c_str());
			}
			else {
				current_card->phonetic_name.last = mm[1];
			}
		}
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::nick_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	bool is_to_decode{ false };
	if (line.contains("ENCODING=QUOTED-PRINTABLE")) {
		is_to_decode = true;
		current_card->nickname.set_encoded_state();
	}

	//////////////////////////////////////////////
	//X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/nickname;=D0
	std::regex nick_pattern("item/nickname;([^;]*)");

	if (std::regex_search(line, mm, nick_pattern)) {
		if (is_to_decode) {
			current_card->nickname.nick = decode(mm[1].str().c_str());
		}
		else {
			current_card->nickname.nick = mm[1];
		}
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::tel_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	///////////////////////////////////
//TEL;X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,=D0):33333
	std::regex tel_pattern(
		"^TEL;X-CUSTOM\\(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,(.*)\\):(.*)$"
	);

	if (std::regex_search(line, mm, tel_pattern)) {
		std::unique_ptr<Telephones> new_tel(new Telephones);

		new_tel->type = decode(mm[1].str().c_str());

		new_tel->number = mm[2].str().c_str();

		new_tel->set_encoded_state();

		current_card->tels.push_back(std::move(*new_tel));
		return;
	}
	//////////////////////////////////////////
//TEL;VOICE:77777
//TEL;X-CustomisoNum:66666
	tel_pattern.assign("^TEL;([^:]+):(.+)$");

	if (std::regex_search(line, mm, tel_pattern)) {
		std::unique_ptr<Telephones> new_tel(new Telephones);

		std::string decoded_typename(mm[1].str());
		if (decoded_typename[0] == 'X' && decoded_typename[1] == '-') {
			decoded_typename = decoded_typename.substr(2);
			new_tel->is_custom = true;
		}

		new_tel->type = decoded_typename;
		new_tel->number = mm[2].str();

		current_card->tels.push_back(std::move(*new_tel));
		return;
	}
	/////////////////////////////////////
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::email_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	/////////////////////////////////////
	//EMAIL;X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,=D0=9E=D1=81=D0=BE=D0=B1=D0=9F=D0=BE=D1=87=D1=82=D0=B0);CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=BF
	std::regex email_pattern("^EMAIL;X-CUSTOM\\([^,]*,[^,]*,(.*)\\);[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, email_pattern)) {
		std::unique_ptr<Emails> new_email(new Emails);

		new_email->type = decode(mm[1].str().c_str());

		new_email->address = decode(mm[2].str().c_str());

		new_email->set_encoded_state();
		new_email->is_custom = true;

		current_card->emails.push_back(std::move(*new_email));
		return;
	}

	/////////////////////////////////////
	//EMAIL;HOME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=AD
	email_pattern.assign("^EMAIL;([^;]*);[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, email_pattern)) {
		std::unique_ptr<Emails> new_email(new Emails);

		new_email->type = mm[1].str();

		new_email->address = decode(mm[2].str().c_str());

		new_email->set_encoded_state();

		current_card->emails.push_back(std::move(*new_email));
		return;
	}

	/////////////////////////////////////
	//EMAIL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=B4
	email_pattern.assign("^EMAIL;[^;]*;[^:]*:(.*)$");

	if (std::regex_search(line, mm, email_pattern)) {
		std::unique_ptr<Emails> new_email(new Emails);

		new_email->type = "";

		new_email->address = decode(mm[1].str().c_str());

		new_email->set_encoded_state();

		current_card->emails.push_back(std::move(*new_email));
		return;
	}

	/////////////////////////////////////
	//EMAIL;WORK:workmail
	email_pattern.assign("^EMAIL;([^;]+):(.+)$");

	if (std::regex_search(line, mm, email_pattern)) {
		std::unique_ptr<Emails> new_email(new Emails);

		new_email->type = mm[1].str();
		new_email->address = mm[2].str();

		current_card->emails.push_back(std::move(*new_email));
		return;
	}

	/////////////////////////////////////
	//EMAIL:email1
	email_pattern.assign("^EMAIL:(.*)$");

	if (std::regex_search(line, mm, email_pattern)) {
		std::unique_ptr<Emails> new_email(new Emails);

		new_email->type = "";
		new_email->address = mm[1].str();

		current_card->emails.push_back(std::move(*new_email));
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::address_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::unique_ptr<Addresses> new_address(new Addresses);

	////////////////////////////////////
	//ADR;WORK:;;Street;City;Region;Index;Country
	std::regex address_pattern("^ADR;([^;]*):;;([^;]*);([^;]*);([^;]*);([^;]*);([^;]*)$");
	std::smatch mm;

	if (std::regex_search(line, mm, address_pattern)) {
		new_address->type = mm[1];
		new_address->street = mm[2];
		new_address->city = mm[3];
		new_address->region = mm[4];
		new_address->index = mm[5];
		new_address->country = mm[6];

		current_card->addresses.push_back(*new_address);
		return;
	}

	////////////////////////////////////
	//ADR;HOME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;=D0;=D0;=D0;=D0;=D0
	address_pattern.assign("^ADR;([^;]*);[^;]*;[^;]*:;;([^;]*);([^;]*);([^;]*);([^;]*);([^;]*)$");

	if (std::regex_search(line, mm, address_pattern)) {
		new_address->type = mm[1];

		std::vector<std::string> scric;

		for (size_t i = 2; i < mm.size(); ++i) {
			scric.push_back(decode(mm[i].str().c_str()));
		}

		new_address->street = scric[0];
		new_address->city = scric[1];
		new_address->region = scric[2];
		new_address->index = scric[3];
		new_address->country = scric[4];

		new_address->set_encoded_state();

		current_card->addresses.push_back(*new_address);
		return;
	}

	////////////////////////////////////
	//ADR;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;=D0;=D0;=D0;=D0;=D0
	address_pattern.assign("^ADR;[^;]*;[^;]*:;;([^;]*);([^;]*);([^;]*);([^;]*);([^;]*)$");

	if (std::regex_search(line, mm, address_pattern)) {
		new_address->type = "";

		std::vector<std::string> scric;

		for (size_t i = 1; i < mm.size(); ++i) {
			scric.push_back(decode(mm[i].str().c_str()));
		}

		new_address->street = scric[0];
		new_address->city = scric[1];
		new_address->region = scric[2];
		new_address->index = scric[3];
		new_address->country = scric[4];

		new_address->set_encoded_state();

		current_card->addresses.push_back(*new_address);
		return;
	}

	////////////////////////////////////
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::company_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	////////////////////////////////////
	//ORG;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=9A
	//std::regex pattern1("^ORG;[^;]*;[^;]*:(.*)$");
	std::regex company_pattern("^ORG;[^;]*;[^;]*:([^;]*)=3B([^;]*)");

	if (std::regex_search(line, mm, company_pattern)) {
		current_card->workinfo.company = decode(mm[1].str().c_str());
		current_card->workinfo.department = decode(mm[2].str().c_str());
		current_card->workinfo.set_encoded_state();

		return;
	}

	////////////////////////////////////
	//ORG:Company;Department
	company_pattern.assign("^ORG:([^;]*);([^;]*)$");

	if (std::regex_search(line, mm, company_pattern)) {
		current_card->workinfo.company = mm[1].str().c_str();

		current_card->workinfo.department = mm[2].str().c_str();

		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::title_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	////////////////////////////////////
	//TITLE;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=94
	std::regex title_pattern("^TITLE;[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, title_pattern)) {
		current_card->workinfo.title = decode(mm[1].str().c_str());
		current_card->workinfo.set_encoded_state();
		return;
	}

	////////////////////////////////////
	//TITLE:Position
	title_pattern.assign("^TITLE:(.*)$");

	if (std::regex_search(line, mm, title_pattern)) {
		current_card->workinfo.title = mm[1].str().c_str();
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::url_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	////////////////////////////////////
	//URL:www.place.org
	std::regex url_pattern("^URL:(.*)$");

	if (std::regex_search(line, mm, url_pattern)) {
		UrlString temp;
		temp.url_address = mm[1].str();
		current_card->urls.push_back(temp);
		return;
	}

	////////////////////////////////////
	//URL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0
	url_pattern.assign("^URL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:(.*)$");
	//"^URL;[^;]*;[^;]*:(.*)$";

	if (std::regex_search(line, mm, url_pattern)) {
		UrlString temp;
		temp.url_address = decode(mm[1].str().c_str());
		temp.set_encoded_state();
		current_card->urls.push_back(temp);
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::note_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	////////////////////////////////////
	//NOTE:Write notes here
	std::regex note_pattern("^NOTE:(.*)$");

	if (std::regex_search(line, mm, note_pattern)) {
		current_card->note.note_text = mm[1];
		return;
	}

	////////////////////////////////////
	//NOTE;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0
	//std::regex pattern2("^NOTE;[^;]*;[^;]*:(.*)$");
	note_pattern.assign("^NOTE;[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, note_pattern)) {
		current_card->note.note_text = decode(mm[1].str().c_str());
		current_card->note.set_encoded_state();
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::events_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;
	std::unique_ptr<Event> event(new Event);

	std::regex event_pattern01("(\\d{4})-(\\d{2})-(\\d{2})");
	std::regex event_pattern02("--(\\d{2})-(\\d{2})");

	auto num_parse = [&](const std::string& eventname, const std::string& toparse) {
		if (std::regex_search(toparse, mm, event_pattern01)) {
			event->event_name = eventname;
			event->year = mm[1].str();
			event->month = mm[2].str();
			event->day = mm[3].str();
			current_card->events.push_back(std::move(*event));
		}
		else if (std::regex_search(toparse, mm, event_pattern02)) {
			event->event_name = eventname;
			event->month = mm[1].str();
			event->day = mm[2].str();
			current_card->events.push_back(std::move(*event));
		}
		};

	//BDAY:2000-01-01
	////////////////////////////////////
	std::regex event_pattern("^BDAY:(\\d{4})-(\\d{2})-(\\d{2})");
	if (std::regex_search(line, mm, event_pattern)) {
		event->event_name = "Birthday";
		event->year = mm[1].str();
		event->month = mm[2].str();
		event->day = mm[3].str();
		event->event_type = EventType::BDAY;
		current_card->events.push_back(std::move(*event));
		return;
	}

	//X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;2024-01-12;0;SpecDate;;;;;;;;;;;;0
	////////////////////////////////////
	event_pattern.assign("contact_event;(.*);0;([^;]*)");

	if (std::regex_search(line, mm, event_pattern)) {
		std::string full_event = mm[1].str();
		event->event_type = EventType::SPEC;
		num_parse(mm[2].str(), full_event);
		return;
	}

	//X-ANDROID-CUSTOM:vnd.android.cursor.item/contact_event;--01-12;1;;;;;;;;;;;;;0
	////////////////////////////////////
	event_pattern.assign("contact_event;(.*);1;");

	if (std::regex_search(line, mm, event_pattern)) {
		std::string full_event = mm[0].str();
		event->event_type = EventType::ANNIV;
		num_parse("Anniversary", full_event);
		return;
	}

	//X - ANDROID - CUSTOM:vnd.android.cursor.item / contact_event; --04 - 12; 2;;;;;;;;;;;;; 0
	////////////////////////////////////
	event_pattern.assign("contact_event;(.*);2;");

	if (std::regex_search(line, mm, event_pattern)) {
		std::string full_event = mm[0].str();
		event->event_type = EventType::OTHER;
		num_parse("Other", full_event);
		return;
	}

	//X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/contact_event;=2D=2D=30=36=2D=31=35;=30;=D0=92=D0=B0=D0=B6=D0=BD=D0=B0=D1=8F=20=D0=94=D0=B0=D1=82=D0=B0=23=E2=82=BD;;;;;;;;;;;;=30
	////////////////////////////////////
	event_pattern.assign("CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/contact_event;([^;]*);[^;]*;([^;]*);");

	if (std::regex_search(line, mm, event_pattern)) {
		event->event_type = EventType::ENCODED;
		event->set_encoded_state();
		num_parse(decode(mm[2].str().c_str()),
			decode(mm[1].str().c_str()));
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

bool VcfParser::is_socials_line(const std::string& line, std::string& sns_name) {
	for (const auto& sns : socials_names) {
		if (line.contains(sns)) {
			sns_name = sns;
			return true;
		}
	}
	return false;
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::socials_parser(const std::string& line, std::unique_ptr<ContactData>& current_card, const std::string& sns_name) {
	std::smatch mm;
	std::unique_ptr<SocialNet> sns(new SocialNet);

	std::string newname;
	if (sns_name == "X-SKYPE-USERNAME") {
		newname = "SKYPE";
	}
	else if (sns_name == "X-GOOGLE-TALK") {
		newname = "HANGOUTS";
	}
	else {
		newname = sns_name.substr(2);
	}
	std::regex socials_pattern;

	//X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,=D0=9E);CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=9A
	socials_pattern.assign("^X-CUSTOM[^,]*,[^,]*,([^;\\)]*)\\);[^;]*;[^;:]*:(.*)$");

	if (std::regex_search(line, mm, socials_pattern)) {
		sns->name = decode(mm[1].str().c_str());
		sns->contact = decode(mm[2].str().c_str());
		sns->is_custom = true;
		sns->set_encoded_state();

		current_card->socials.push_back(*sns);
		return;
	}

	// X-JABBER;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=90=D0=B4
	socials_pattern.assign("^X-[^;]*;[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, socials_pattern)) {
		sns->name = newname;

		sns->contact = decode(mm[1].str().c_str());
		sns->set_encoded_state();

		current_card->socials.push_back(*sns);
		return;
	}

	//X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,Special Sns):this sns
	socials_pattern.assign("^X-CUSTOM\\(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,(.*)\\):(.*)$");

	if (std::regex_search(line, mm, socials_pattern)) {
		sns->name = mm[1];
		sns->contact = mm[2];
		sns->is_custom = true;
		current_card->socials.push_back(*sns);
		return;
	}

	//X-WHATSAPP:whazzzzap
	socials_pattern.assign("^X-[^;:]*:(.*)$");

	if (std::regex_search(line, mm, socials_pattern)) {
		sns->name = newname;
		sns->contact = mm[1];

		current_card->socials.push_back(*sns);
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void VcfParser::relations_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;
	std::unique_ptr<Relation> relate(new Relation);

	std::string type_name;

	auto select_type = [&type_name](const int& type_number, const std::string& custom_type_name = "") {
		switch (type_number) {
		case 1:
			type_name = "Assistant";
			break;
		case 2:
			type_name = "Brother";
			break;
		case 3:
			type_name = "Child";
			break;
		case 4:
			type_name = "Partner";
			break;
		case 5:
			type_name = "Father";
			break;
		case 6:
			type_name = "Friend";
			break;
		case 7:
			type_name = "Boss";
			break;
		case 8:
			type_name = "Mother";
			break;
		case 9:
			type_name = "Close Relative";
			break;
		case 10:
			type_name = "Work Partner";
			break;
		case 11:
			type_name = "Recommendation";
			break;
		case 12:
			type_name = "Relative";
			break;
		case 13:
			type_name = "Sister";
			break;
		case 14:
			type_name = "Spouse";
			break;
		default:
			type_name = (custom_type_name.empty()) ? type_name = "Other" : custom_type_name;
			break;
		}
		};

	std::regex relations_pattern;
	///////////////////////////////////////
	relations_pattern.assign("^[^;]*relation;([^;]*);([^;]*);([^;]*);");

	if (std::regex_search(line, mm, relations_pattern)) {
		relate->name = mm[1];
		std::string iii = mm[2].str();
		relate->type_num = std::stoi(iii);
		type_name = mm[3];
		select_type(relate->type_num, type_name);
		relate->type_name = type_name;

		current_card->relations.push_back(*relate);
		return;
	}
	////////////////////////////////////

	relations_pattern.assign("^[^;]*;[^;]*;[^;]*relation;([^;]*);([^;]*);([^;]*);");

	if (std::regex_search(line, mm, relations_pattern)) {
		relate->name = decode(mm[1].str().c_str());

		relate->type_num = std::stoi(decode(mm[2].str().c_str()));

		if (mm[3].str() != "") {
			type_name = decode(mm[3].str().c_str());
		}
		select_type(relate->type_num, type_name);
		relate->type_name = type_name;
		relate->set_encoded_state();

		current_card->relations.push_back(*relate);
		return;
	}
}