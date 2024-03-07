#pragma once
#ifndef VCFE_DATA_CONTAINERS_H
#define VCFE_DATA_CONTAINERS_H

#include <string>
#include <vector>

class VcfEntry {
public:
	~VcfEntry() = default;
	virtual bool is_empty() const = 0;
};

struct NameRecord : public VcfEntry {
	std::string family;
	std::string personal;
	std::string father;
	std::string address_form;
	std::string suffix;

	inline bool is_empty() const override {
		return family.empty() &&
			personal.empty() &&
			father.empty() &&
			address_form.empty() &&
			suffix.empty();
	}
};

struct PhoneticRecord : public VcfEntry {
	std::string first;
	std::string middle;
	std::string last;

	inline bool is_empty() const {
		return first.empty() &&
			middle.empty() &&
			last.empty();
	}
};

struct NickNameRecord : public VcfEntry {
	std::string nick;
	inline bool is_empty() const override {
		return nick.empty();
	}
};

const std::vector<std::string> telephone_types{
	"CELL",
	"HOME",
	"HOME;FAX",
	"WORK",
	"WORK;FAX",
	"VOICE",
	"PAGER",
	"CALLBACK"
};

struct TelephoneRecord : public VcfEntry {
	std::string type;
	std::string number;
	inline bool is_empty() const override {
		return number.empty();
	}
};

struct EmailRecord : public VcfEntry {
	std::string type;
	std::string address;
	inline bool is_empty() const override {
		return address.empty();
	}
};

struct AddressRecord : public VcfEntry {
	std::string type;
	std::string street;
	std::string city;
	std::string region;
	std::string index;
	std::string country;
	inline bool is_empty() const override {
		return street.empty() &&
			city.empty() &&
			region.empty() &&
			index.empty() &&
			country.empty();
	}
};

enum class EventType {
	NONE		= -1,
	SPECIAL		=  0, 
	ANNIVERSARY	=  1, 
	OTHER		=  2,
	BDAY		=  3
};

struct EventRecord : public VcfEntry {
	std::string event_name;
	std::string day;
	std::string month;
	std::string year;
	EventType event_type;
	inline bool is_empty() const override {
		return day.empty() &&
			month.empty();
	}
};

struct WorkInfoRecord : public VcfEntry {
	std::string company;
	std::string department;
	std::string title;

	inline bool is_empty() const {
		return (company.empty()
			&& department.empty()
			&& title.empty());
	}
};

struct UrlRecord : public VcfEntry {
	std::string url_address;
	inline bool is_empty() const override {
		return url_address.empty();
	}
};

struct NoteRecord : public VcfEntry {
	std::string note_text;
	inline bool is_empty() const override {
		return note_text.empty();
	}
};

const std::vector<std::string> socials_names{
	"WHATSAPP",
	"FACEBOOK",
	"GOOGLE-TALK",
	"QQ",
	"SKYPE-USERNAME",
	"YAHOO",
	"AIM",
	"ICQ",
	"JABBER",
	"MSN",
	"CUSTOM("
};

struct SocialNetRecord : public VcfEntry {
	std::string name;
	std::string contact;
	inline bool is_empty() const override {
		return contact.empty();
	}
};

struct RelationRecord : public VcfEntry {
	std::string name;
	std::string type_name;
	int type_num;
	inline bool is_empty() const override {
		return name.empty() || type_name.empty();
	}
};

struct ContactData {
	std::string version;
	NameRecord names;
	PhoneticRecord phonetic_name;
	NickNameRecord nickname;
	std::vector<TelephoneRecord> telephones;
	std::vector<EmailRecord> emails;
	std::vector<AddressRecord> addresses;
	WorkInfoRecord workinfo;
	std::vector<UrlRecord> urls;
	NoteRecord note;
	std::vector<EventRecord> events;
	std::vector<SocialNetRecord> socials;
	std::vector<RelationRecord> relations;
};

#endif // !VCFE_DATA_CONTAINERS_H
