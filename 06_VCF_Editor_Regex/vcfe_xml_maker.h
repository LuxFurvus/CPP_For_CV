
#pragma once
#ifndef VCFE_XML_MAKER_H
#define VCFE_XML_MAKER_H

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "vcfe_data_containers.h"

class XmlMakerBoost {
private:
	boost::property_tree::ptree card_tree;

	void xml_names_boost(const NameRecord& names, boost::property_tree::ptree& pt);
	void xml_phonetic_name_boost(const PhoneticRecord& phoneticName, boost::property_tree::ptree& pt);
	void xml_nick_name_boost(const NickNameRecord& nickName, boost::property_tree::ptree& pt);
	void xml_telephone_boost(const TelephoneRecord& telephones, boost::property_tree::ptree& pt);
	void xml_emails_boost(const EmailRecord& emails, boost::property_tree::ptree& pt);
	void xml_address_boost(const AddressRecord& addresses, boost::property_tree::ptree& pt);
	void xml_event_boost(const EventRecord& event, boost::property_tree::ptree& pt);
	void xml_work_info_boost(const WorkInfoRecord& work_info, boost::property_tree::ptree& pt);
	void xml_url_boost(const UrlRecord& url_record, boost::property_tree::ptree& pt);
	void xml_note_boost(const NoteRecord& note, boost::property_tree::ptree& pt);
	void xml_social_net_boost(const SocialNetRecord& social_net, boost::property_tree::ptree& pt);
	void xml_relation_boost(const RelationRecord& relation_record, boost::property_tree::ptree& pt);
public:

	inline void print_xml_to_file(const std::string& filename) const noexcept {
		std::ofstream output_file(filename);
		if (!output_file.is_open()) return;
		boost::property_tree::xml_writer_settings<std::string> settings(' ', 2);
		boost::property_tree::write_xml(output_file, card_tree, settings);
		output_file.close();
	}
	inline void print_xml_to_console() const noexcept {
		boost::property_tree::xml_writer_settings<std::string> settings(' ', 2);
		boost::property_tree::write_xml(std::cout, card_tree, settings);
	}
	inline const boost::property_tree::ptree& get_ptree() const noexcept {
		return card_tree;
	}

	void make_xml(const std::vector<ContactData>& cards);
};

#endif // VCFE_XML_MAKER_H
