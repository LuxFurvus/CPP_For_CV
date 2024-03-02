#pragma once
#ifndef VCFE_SERIALIZER_H
#define VCFE_SERIALIZER_H

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "vcfe_data_containers.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

class IBaseSerializer {
public:
	virtual ~IBaseSerializer() = default;

	using Cards = std::vector<ContactData>;

	// Virtual functions for output
	virtual void print_to_file(const std::string& filename) const noexcept = 0;
	virtual void print_to_console() const noexcept = 0;
};

class BoostSerializer {
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

	void deserialize_name(NameRecord& names, const ptree& node);
	void deserialize_phonetic(PhoneticRecord& phonetic_names, const ptree& node);
	void deserialize_nick_name(NickNameRecord& nickname, const ptree& node);
	void deserialize_note(NoteRecord& note, const ptree& node);
	void deserialize_work_info(WorkInfoRecord& work_info, const ptree& node);
	void deserialize_telephone(TelephoneRecord& telephones, const ptree& node);
	void deserialize_email(EmailRecord& emails, const ptree& node);
	void deserialize_address(AddressRecord& addresses, const ptree& node);
	void deserialize_event(EventRecord& event, const ptree& node);
	void deserialize_url(UrlRecord& url, const ptree& node);
	void deserialize_social_net(SocialNetRecord& social_net, const ptree& node);
	void deserialize_relation(RelationRecord& relation, const ptree& node);


protected:
	inline const ptree& get_ptree() const noexcept {
		return card_tree;
	}
public:
	virtual ~BoostSerializer() = default;

	void serialize_vcards(const std::vector<ContactData>& cards);
	void deserialize_vcards(const std::string& filename, std::vector<ContactData>& cards);

	// Declaration for the deserialize_names function
	void deserialize_names(const boost::property_tree::ptree& pt, NameRecord& names);

	// Declaration for the deserialize_telephone function
	void deserialize_telephone(const boost::property_tree::ptree& pt, TelephoneRecord& telephone);
};



#endif // VCFE_SERIALIZER_H
