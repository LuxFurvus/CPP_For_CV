#pragma once
#ifndef VCFE_REGEX_PARSERS_H
#define VCFE_REGEX_PARSERS_H

#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <array>

struct Names {
	std::string family;
	std::string personal;
	std::string father;
	std::string address_form;
	std::string suffix;

	inline bool is_empty() const {
		return family.empty() &&
			personal.empty() &&
			father.empty() &&
			address_form.empty() &&
			suffix.empty();
	}

	inline std::string& operator[](int n) {
		switch (n) {
		default:
		case 0: return family;
		case 1: return personal;
		case 2: return father;
		case 3: return address_form;
		case 4: return suffix;
		}
	}

	inline std::string const& operator[](int n) const {
		switch (n) {
		default:
		case 0: return family;
		case 1: return personal;
		case 2: return father;
		case 3: return address_form;
		case 4: return suffix;
		}
	}
};

struct PhoneticName {
	std::string first;
	std::string middle;
	std::string last;

	inline bool is_empty() const {
		return first.empty() &&
			middle.empty() &&
			last.empty();
	}
};

struct Telephones {
	std::string type;
	std::string number;
};

struct Emails {
	std::string type;
	std::string address;
};

struct Addresses {
	std::string type;
	std::string street;
	std::string city;
	std::string region;
	std::string index;
	std::string country;

	inline std::string& operator[](int n) {
		switch (n) {
		default:
		case 0: return type;
		case 1: return street;
		case 2: return city;
		case 3: return region;
		case 4: return index;
		case 5: return country;
		}
	}

	inline std::string const& operator[](int n) const {
		switch (n) {
		default:
		case 0: return type;
		case 1: return street;
		case 2: return city;
		case 3: return region;
		case 4: return index;
		case 5: return country;
		}
	}
};

struct Event {
	std::string event_name;
	std::string day;
	std::string month;
	std::string year;
};

struct SocialNet {
	std::string name;
	std::string contact;
};

struct WorkInfo {
	std::string company;
	std::string department;
	std::string title;

	inline bool is_empty() const {
		return (company.empty()
			&& department.empty()
			&& title.empty());
	}
};

struct Relation {
	std::string name;
	std::string type_name;
	int type_num;
};

const std::vector<std::string> socials_names{
	"X-WHATSAPP",
	"X-FACEBOOK",
	"X-GOOGLE-TALK",
	"X-QQ",
	"X-SKYPE-USERNAME",
	"X-YAHOO",
	"X-AIM",
	"X-ICQ",
	"X-JABBER",
	"X-MSN",
	"X-CUSTOM("
};

struct ContactData {
	Names names;
	PhoneticName phonetic_name;
	std::string nickname;
	std::vector<Telephones> tels;
	std::vector<Emails> emails;
	std::vector<Addresses> addresses;
	WorkInfo workinfo;
	std::vector<std::string> urls;
	std::string note;
	std::vector<Event> events;
	std::vector<SocialNet> socials;
	std::vector<Relation> relations;
};

void tel_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
void name_parser(const std::string& line, std::unique_ptr<ContactData>& current_card);
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
