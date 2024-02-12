#pragma once
#ifndef VCFE_DATA_CONTAINERS_H
#define VCFE_DATA_CONTAINERS_H

#include <string>
#include <vector>

class VcfEntry {
private: bool encoded{ false };

public: bool is_encoded() const noexcept {
	return encoded;
}
public: void set_encoded_state() {
	encoded = true;
}
};


struct Names : public VcfEntry {
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

struct PhoneticName : public VcfEntry {
	std::string first;
	std::string middle;
	std::string last;

	inline bool is_empty() const {
		return first.empty() &&
			middle.empty() &&
			last.empty();
	}
};

struct NickName : public VcfEntry {
public: std::string nick;
};

struct Telephones : public VcfEntry {
	std::string type;
	std::string number;
};

struct Emails : public VcfEntry {
	std::string type;
	std::string address;
};

struct Addresses : public VcfEntry {
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

struct Event : public VcfEntry {
	std::string event_name;
	std::string day;
	std::string month;
	std::string year;
};

struct SocialNet : public VcfEntry {
	std::string name;
	std::string contact;
};

struct WorkInfo : public VcfEntry {
	std::string company;
	std::string department;
	std::string title;

	inline bool is_empty() const {
		return (company.empty()
			&& department.empty()
			&& title.empty());
	}
};

struct Relation : public VcfEntry {
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
	std::string version;
	Names names;
	PhoneticName phonetic_name;
	NickName nickname;
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

#endif // !VCFE_DATA_CONTAINERS_H
