extern "C" {
#include "vcfe_deencode_funcs.h"
}

#include "vcfe_regex_parsers.h"

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void name_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;
	/////////////////////////////////////
	//N:FamilyName;PersonalName;FatherName;AppealForm;NameSuffix
	//N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=A4;=D0=98;=D0=9E;=D0=A4;=D0=A1
	std::regex pattern1("^N[^:]*:(?:[^:]*:)*([^;]*);([^;]*);([^;]*);([^;]*);([^;]*)$");

	bool is_to_decode = (line.contains("ENCODING=QUOTED-PRINTABLE")) ? true : false;

	if (std::regex_search(line, mm, pattern1)) {
		// Extract names from the matched parts
		for (size_t i = 1; i < mm.size(); ++i) {
			if (mm[i].str().empty()) {
				continue;
			}
			if (is_to_decode) {
				char newname_buffer[512];
				decode_hex_string(mm[i].str().c_str(), newname_buffer);
				std::string decoded_string(newname_buffer);
				current_card->names[i] = decoded_string;
			}
			else {
				current_card->names[i] = mm[i].str();
			}
		}
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void phonetics_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;
	bool is_to_decode = (line.contains("ENCODING=QUOTED-PRINTABLE")) ? true : false;
	///////////////////////////////////
//X-PHONETIC-FIRST-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0
//X-PHONETIC-MIDDLE-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0
//X-PHONETIC-LAST-NAME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0

	std::regex phonetic_pattern("^X-PHONETIC-[^:]*:([^;]*)$");

	if (std::regex_search(line, mm, phonetic_pattern)) {
		if (line.contains("X-PHONETIC-FIRST-NAME")) {
			if (is_to_decode) {
				char newphonetic_buffer[512];
				decode_hex_string(mm[1].str().c_str(), newphonetic_buffer);
				std::string decoded_string(newphonetic_buffer);
				current_card->phonetic_name.first = decoded_string;
			}
			else {
				current_card->phonetic_name.first = mm[0];
			}
		}
		else if (line.contains("X-PHONETIC-MIDDLE-NAME")) {
			if (is_to_decode) {
				char newphonetic_buffer[512];
				decode_hex_string(mm[1].str().c_str(), newphonetic_buffer);
				std::string decoded_string(newphonetic_buffer);
				current_card->phonetic_name.middle = decoded_string;
			}
			else {
				current_card->phonetic_name.middle = mm[0];
			}
		}
		else if (line.contains("X-PHONETIC-LAST-NAME")) {
			if (is_to_decode) {
				char newphonetic_buffer[512];
				decode_hex_string(mm[1].str().c_str(), newphonetic_buffer);
				std::string decoded_string(newphonetic_buffer);
				current_card->phonetic_name.last = decoded_string;
			}
			else {
				current_card->phonetic_name.last = mm[0];
			}
		}
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void nick_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;
	bool is_to_decode = (line.contains("ENCODING=QUOTED-PRINTABLE")) ? true : false;

	//////////////////////////////////////////////
	//X-ANDROID-CUSTOM;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/nickname;=D0
	std::regex nick_pattern("item/nickname;([^;]*)");

	if (std::regex_search(line, mm, nick_pattern)) {
		if (is_to_decode) {
			char newname_buffer[512];
			decode_hex_string(mm[1].str().c_str(), newname_buffer);
			std::string decoded_string(newname_buffer);
			current_card->nickname = decoded_string;
		}
		else {
			current_card->nickname = mm[1];
		}
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void tel_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	///////////////////////////////////
//TEL;X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,=D0):33333
	std::regex pattern1("^TEL;X-CUSTOM\\([^,]*,[^,]*,(.*)\\):(.*)$");

	if (std::regex_search(line, mm, pattern1)) {
		std::unique_ptr<Telephones> new_tel(new Telephones);

		char typename_buffer[512];
		decode_hex_string(mm[1].str().c_str(), typename_buffer);
		std::string decoded_typename(typename_buffer);
		decoded_typename.erase(decoded_typename.size() - 1);

		new_tel->type = decoded_typename;
		new_tel->number = mm[2].str();

		current_card->tels.push_back(std::move(*new_tel));
		return;
	}
	//////////////////////////////////////////
//TEL;VOICE:77777
//TEL;X-CustomisoNum:66666
	std::regex pattern2("^TEL;([^:]+):(.+)$");

	if (std::regex_search(line, mm, pattern2)) {
		std::unique_ptr<Telephones> new_tel(new Telephones);

		std::string decoded_typename(mm[1].str());
		if (decoded_typename[0] == 'X' && decoded_typename[1] == '-') {
			decoded_typename = decoded_typename.substr(2);
		}

		new_tel->type = decoded_typename;
		new_tel->number = mm[2].str();

		current_card->tels.push_back(std::move(*new_tel));
		return;
	}
	/////////////////////////////////////
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void email_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	/////////////////////////////////////
	//EMAIL;X-CUSTOM(CHARSET=UTF-8,ENCODING=QUOTED-PRINTABLE,=D0=9E=D1=81=D0=BE=D0=B1=D0=9F=D0=BE=D1=87=D1=82=D0=B0);CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=BF
	std::regex pattern1("^EMAIL;X-CUSTOM\\([^,]*,[^,]*,(.*)\\);[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, pattern1)) {
		std::unique_ptr<Emails> new_email(new Emails);

		char typename_buffer[512];
		decode_hex_string(mm[1].str().c_str(), typename_buffer);
		std::string decoded_type(typename_buffer);
		new_email->type = decoded_type;

		char newname_buffer[512];
		decode_hex_string(mm[2].str().c_str(), newname_buffer);
		std::string decoded_address(newname_buffer);
		new_email->address = decoded_address;

		current_card->emails.push_back(std::move(*new_email));
		return;
	}

	/////////////////////////////////////
	//EMAIL;HOME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=AD
	std::regex pattern3("^EMAIL;([^;]*);[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, pattern3)) {
		std::unique_ptr<Emails> new_email(new Emails);

		new_email->type = mm[1].str();

		char newname_buffer[512];
		decode_hex_string(mm[2].str().c_str(), newname_buffer);
		std::string decoded_string(newname_buffer);

		new_email->address = decoded_string;

		current_card->emails.push_back(std::move(*new_email));
		return;
	}

	/////////////////////////////////////
	//EMAIL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=B4
	std::regex pattern2("^EMAIL;[^;]*;[^:]*:(.*)$");

	if (std::regex_search(line, mm, pattern2)) {
		std::unique_ptr<Emails> new_email(new Emails);

		new_email->type = "OTHER";

		char newname_buffer[512];
		decode_hex_string(mm[1].str().c_str(), newname_buffer);
		std::string decoded_address(newname_buffer);
		new_email->address = decoded_address;

		current_card->emails.push_back(std::move(*new_email));
		return;
	}

	/////////////////////////////////////
	//EMAIL;WORK:workmail
	std::regex pattern4("^EMAIL;([^;]+):(.+)$");

	if (std::regex_search(line, mm, pattern4)) {
		std::unique_ptr<Emails> new_email(new Emails);

		new_email->type = mm[1].str();
		new_email->address = mm[2].str();

		current_card->emails.push_back(std::move(*new_email));
		return;
	}

	/////////////////////////////////////
	//EMAIL:email1
	std::regex pattern5("^EMAIL:(.*)$");

	if (std::regex_search(line, mm, pattern5)) {
		std::unique_ptr<Emails> new_email(new Emails);

		new_email->type = "OTHER";
		new_email->address = mm[1].str();

		current_card->emails.push_back(std::move(*new_email));
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void address_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::unique_ptr<Addresses> new_address(new Addresses);

	////////////////////////////////////
	//ADR;WORK:;;Street;City;Region;Index;Country
	std::regex address_pattern1("^ADR;([^;]*):;;([^;]*);([^;]*);([^;]*);([^;]*);([^;]*)$");
	std::smatch mm;

	if (std::regex_search(line, mm, address_pattern1)) {
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
	std::regex address_pattern2("^ADR;([^;]*);[^;]*;[^;]*:;;([^;]*);([^;]*);([^;]*);([^;]*);([^;]*)$");

	if (std::regex_search(line, mm, address_pattern2)) {
		new_address->type = mm[1];

		std::vector<std::string> scric;

		for (size_t i = 2; i < mm.size(); ++i) {
			char buffer1[512];
			decode_hex_string(mm[i].str().c_str(), buffer1);
			std::string decoded_string(buffer1);
			scric.push_back(decoded_string);
		}

		new_address->street = scric[0];
		new_address->city = scric[1];
		new_address->region = scric[2];
		new_address->index = scric[3];
		new_address->country = scric[4];

		current_card->addresses.push_back(*new_address);
		return;
	}

	////////////////////////////////////
	//ADR;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;=D0;=D0;=D0;=D0;=D0
	std::regex address_pattern3("^ADR;[^;]*;[^;]*:;;([^;]*);([^;]*);([^;]*);([^;]*);([^;]*)$");

	if (std::regex_search(line, mm, address_pattern3)) {
		new_address->type = "OTHER";

		std::vector<std::string> scric;

		for (size_t i = 1; i < mm.size(); ++i) {
			char buffer1[512];
			decode_hex_string(mm[i].str().c_str(), buffer1);
			std::string decoded_string(buffer1);
			scric.push_back(decoded_string);
		}

		new_address->street = scric[0];
		new_address->city = scric[1];
		new_address->region = scric[2];
		new_address->index = scric[3];
		new_address->country = scric[4];

		current_card->addresses.push_back(*new_address);
		return;
	}

	////////////////////////////////////
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void company_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	////////////////////////////////////
	//ORG;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=9A
	std::regex pattern1("^ORG;[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, pattern1)) {
		char newname_buffer[512];
		decode_hex_string(mm[1].str().c_str(), newname_buffer);
		std::string comdep(newname_buffer);

		std::regex pattern1("([^;]*);([^;]*)");
		if (std::regex_search(comdep, mm, pattern1)) {
			current_card->workinfo.company = mm[1].str();
			current_card->workinfo.department = mm[2].str();
		}

		return;
	}

	////////////////////////////////////
	//ORG:Company;Department
	std::regex pattern3("^ORG:([^;]*);([^;]*)$");

	if (std::regex_search(line, mm, pattern3)) {
		char company_buffer[512];
		decode_hex_string(mm[1].str().c_str(), company_buffer);
		std::string decoded_company(company_buffer);

		current_card->workinfo.company = decoded_company;

		char department_buffer[512];
		decode_hex_string(mm[2].str().c_str(), department_buffer);
		std::string decoded_department(department_buffer);

		current_card->workinfo.department = decoded_department;

		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void title_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	////////////////////////////////////
	//TITLE;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0=94
	std::regex pattern2("^TITLE;[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, pattern2)) {
		char newname_buffer[512];
		decode_hex_string(mm[1].str().c_str(), newname_buffer);
		std::string decoded_string(newname_buffer);

		current_card->workinfo.title = decoded_string;
		return;
	}

	////////////////////////////////////
	//TITLE:Position
	std::regex pattern4("^TITLE:(.*)$");

	if (std::regex_search(line, mm, pattern4)) {
		char newname_buffer[512];
		decode_hex_string(mm[1].str().c_str(), newname_buffer);
		std::string decoded_string(newname_buffer);

		current_card->workinfo.title = decoded_string;
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void url_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	////////////////////////////////////
	//URL:www.place.org
	std::regex pattern1("^URL:(.*)$");

	if (std::regex_search(line, mm, pattern1)) {
		current_card->urls.push_back(mm[1]);
		return;
	}

	////////////////////////////////////
	//URL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0
	std::regex pattern2("^URL;[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, pattern2)) {
		char newname_buffer[512];
		decode_hex_string(mm[1].str().c_str(), newname_buffer);
		std::string decoded_string(newname_buffer);

		current_card->urls.push_back(decoded_string);
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void note_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;

	////////////////////////////////////
	//NOTE:Write notes here
	std::regex pattern1("^NOTE:(.*)$");

	if (std::regex_search(line, mm, pattern1)) {
		current_card->note = mm[1];
		return;
	}

	////////////////////////////////////
	//NOTE;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=D0
	std::regex pattern2("^NOTE;[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, pattern2)) {
		char newname_buffer[512];
		decode_hex_string(mm[1].str().c_str(), newname_buffer);
		std::string decoded_string(newname_buffer);

		current_card->note = decoded_string;
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void events_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
	std::smatch mm;
	std::unique_ptr<Event> event(new Event);

	std::regex event_pattern1("(\\d{4})-(\\d{2})-(\\d{2})");
	std::regex event_pattern2("--(\\d{2})-(\\d{2})");

	auto num_parse = [&](const std::string& eventname, const std::string& toparse) {
		if (std::regex_search(toparse, mm, event_pattern1)) {
			event->event_name = eventname;
			event->year = mm[1].str();
			event->month = mm[2].str();
			event->day = mm[3].str();
			current_card->events.push_back(std::move(*event));
		}
		else if (std::regex_search(toparse, mm, event_pattern2)) {
			event->event_name = eventname;
			event->month = mm[1].str();
			event->day = mm[2].str();
			current_card->events.push_back(std::move(*event));
		}
		};

	////////////////////////////////////
	std::regex pattern1("^BDAY:(\\d{4})-(\\d{2})-(\\d{2})");
	if (std::regex_search(line, mm, pattern1)) {
		event->event_name = "Birthday";
		event->year = mm[1].str();
		event->month = mm[2].str();
		event->day = mm[3].str();
		current_card->events.push_back(std::move(*event));
		return;
	}

	////////////////////////////////////
	std::regex pattern2("contact_event;(.*);1;");

	if (std::regex_search(line, mm, pattern2)) {
		std::string full_event = mm[0].str();
		num_parse("Anniversary", full_event);
		return;
	}

	////////////////////////////////////
	std::regex pattern3("contact_event;(.*);2;");

	if (std::regex_search(line, mm, pattern3)) {
		std::string full_event = mm[0].str();
		num_parse("Other", full_event);
		return;
	}

	////////////////////////////////////
	std::regex pattern4("CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:vnd.android.cursor.item/contact_event;([^;]*);[^;]*;([^;]*);");

	if (std::regex_search(line, mm, pattern4)) {
		char newevent_buffer[128];
		decode_hex_string(mm[1].str().c_str(), newevent_buffer);
		std::string decoded_event(newevent_buffer);

		char newname_buffer[512];
		decode_hex_string(mm[2].str().c_str(), newname_buffer);
		std::string decoded_eventname(newname_buffer);

		num_parse(decoded_eventname, decoded_event);
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

bool is_socials_line(const std::string& line, std::string& sns_name) {
	for (const auto& sns : socials_names) {
		if (line.contains(sns)) {
			sns_name = sns;
			return true;
		}
	}
	return false;
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void socials_parser(const std::string& line, std::unique_ptr<ContactData>& current_card, const std::string& sns_name) {
	std::smatch mm;
	std::unique_ptr<SocialNet> sns(new SocialNet);

	std::string newname;
	if (sns_name == "X-SKYPE-USERNAME") {
		newname = "SKYPE";
	}
	else if (sns_name == "X-GOOGLE-TALK") {
		newname = "HANGOUTS";
	}
	else if (sns_name == "X-CUSTOM(") {
		newname = "CUSTOM";
	}
	else {
		newname = sns_name.substr(2);
	}

	std::regex pattern1("^X-[^;:]*:(.*)$");

	if (std::regex_search(line, mm, pattern1)) {
		sns->name = newname;
		sns->contact = mm[1];

		current_card->socials.push_back(*sns);
		return;
	}

	////////////////////////////////////
	std::regex pattern3("^X-CUSTOM[^,]*,[^,]*,([^;\\)]*)\\);[^;]*;[^;:]*:(.*)$");

	if (std::regex_search(line, mm, pattern3)) {
		char sns_name_buffer[512];
		decode_hex_string(mm[1].str().c_str(), sns_name_buffer);
		std::string new_sns_name(sns_name_buffer);

		char contact_buffer[512];
		decode_hex_string(mm[2].str().c_str(), contact_buffer);
		std::string decoded_contact(contact_buffer);

		sns->name = new_sns_name;
		sns->contact = decoded_contact;

		current_card->socials.push_back(*sns);
		return;
	}

	////////////////////////////////////
	std::regex pattern2("^X-[^;]*;[^;]*;[^;]*:(.*)$");

	if (std::regex_search(line, mm, pattern2)) {
		sns->name = newname;

		char newname_buffer[512];
		decode_hex_string(mm[1].str().c_str(), newname_buffer);
		std::string decoded_string(newname_buffer);

		sns->contact = decoded_string;

		current_card->socials.push_back(*sns);
		return;
	}
}

///=///=///=///=///=///=///=///=///=///=///=///=///=///=

void relations_parser(const std::string& line, std::unique_ptr<ContactData>& current_card) {
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

	///////////////////////////////////////
	std::regex pattern1("^[^;]*relation;([^;]*);([^;]*);([^;]*);");

	if (std::regex_search(line, mm, pattern1)) {
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

	std::regex pattern2("^[^;]*;[^;]*;[^;]*relation;([^;]*);([^;]*);([^;]*);");

	if (std::regex_search(line, mm, pattern2)) {
		char relate_name_buffer[512];
		decode_hex_string(mm[1].str().c_str(), relate_name_buffer);
		std::string new_relate_name(relate_name_buffer);
		relate->name = new_relate_name;

		char relate_num_buffer[32];
		decode_hex_string(mm[2].str().c_str(), relate_num_buffer);
		std::string new_relate_num(relate_num_buffer);
		relate->type_num = std::stoi(new_relate_num);

		char relate_type_buffer[512];
		if (mm[3].str() != "") {
			decode_hex_string(mm[3].str().c_str(), relate_type_buffer);
			std::string new_relate_type(relate_type_buffer);
			type_name = new_relate_type;
		}
		select_type(relate->type_num, type_name);
		relate->type_name = type_name;

		current_card->relations.push_back(*relate);
		return;
	}
}