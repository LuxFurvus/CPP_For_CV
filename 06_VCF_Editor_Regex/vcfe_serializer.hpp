#pragma once
#ifndef VCFE_SERIALIZER_H
#define VCFE_SERIALIZER_H

#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "vcfe_data_containers.hpp"

class ICardPrinter {
public:
	virtual ~ICardPrinter() = default;

	virtual void print_to_file(const std::string& filename) const noexcept = 0;
	virtual void print_to_console() const noexcept = 0;
};

class PropertyTreeSerializer {
	using ptree = boost::property_tree::ptree;
protected:
	boost::property_tree::ptree card_tree;
protected:
	void serialize_names(const NameRecord& names, ptree& pt);
	void serialize_phonetic_name(const PhoneticRecord& phonetic_names, ptree& pt);
	void serialize_nick_name(const NickNameRecord& nickname, ptree& pt);
	void serialize_telephone(const TelephoneRecord& telephones, ptree& pt);
	void serialize_emails(const EmailRecord& emails, ptree& pt);
	void serialize_address(const AddressRecord& addresses, ptree& pt);
	void serialize_event(const EventRecord& event, ptree& pt);
	void serialize_work_info(const WorkInfoRecord& work_info, ptree& pt);
	void serialize_url(const UrlRecord& url, ptree& pt);
	void serialize_note(const NoteRecord& note, ptree& pt);
	void serialize_social_net(const SocialNetRecord& social_net, ptree& pt);
	void serialize_relation(const RelationRecord& relation, ptree& pt);

	void deserialize_names(const ptree& pt, NameRecord& names);
	void deserialize_phonetic(const ptree& node, PhoneticRecord& phonetic_names);
	void deserialize_nick_name(const ptree& node, NickNameRecord& nickname);
	void deserialize_work_info(const ptree& node, WorkInfoRecord& work_info);
	void deserialize_note(const ptree& node, NoteRecord& note);
	void deserialize_telephone(const ptree& pt, TelephoneRecord& telephone);
	void deserialize_email(const ptree& pt, EmailRecord& email);
	void deserialize_address(const ptree& pt, AddressRecord& address);
	void deserialize_url(const ptree& pt, UrlRecord& url);
	void deserialize_event(const ptree& node, EventRecord& event);
	void deserialize_social_net(const ptree& node, SocialNetRecord& social_net);
	void deserialize_relation(const ptree& node, RelationRecord& relation);

protected:
	inline const ptree& get_ptree() const noexcept {
		return card_tree;
	}
public:
	virtual ~PropertyTreeSerializer() = default;

	void serialize_vcards(const std::vector<ContactData>& cards);
	void deserialize_vcards(const std::string& filename, std::vector<ContactData>& cards);
};


class JsonMakerBoost : public ICardPrinter, public PropertyTreeSerializer {
public:

	inline void print_to_file(const std::string& filename) const noexcept override {
		std::ofstream output_file(filename);
		if (!output_file.is_open()) return;
		boost::property_tree::write_json(output_file, card_tree, true);
		output_file.close();
	}
	inline void print_to_console() const noexcept override {
		boost::property_tree::write_json(std::cout, card_tree, true);
	}

};

class XmlMakerBoost : public ICardPrinter, public PropertyTreeSerializer {
public:

	inline void print_to_file(const std::string& filename) const noexcept override {
		std::ofstream output_file(filename);
		if (!output_file.is_open()) return;
		boost::property_tree::xml_writer_settings<std::string> settings(' ', 2);
		boost::property_tree::write_xml(output_file, card_tree, settings);
		output_file.close();
	}
	inline void print_to_console() const noexcept override {
		boost::property_tree::xml_writer_settings<std::string> settings(' ', 2);
		boost::property_tree::write_xml(std::cout, card_tree, settings);
	}
};


#endif // VCFE_SERIALIZER_H
