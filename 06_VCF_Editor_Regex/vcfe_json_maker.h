#pragma once
#ifndef VCFE_JSON_MAKER_H
#define VCFE_JSON_MAKER_H

#include <boost/property_tree/ptree.hpp>
#include "vcfe_data_containers.h"

class JsonMakerBoost {
private:
	boost::property_tree::ptree card_tree;

	void json_names_boost(const NameRecord& names, boost::property_tree::ptree& pt);
	void json_phonetic_name_boost(const PhoneticRecord& phoneticName, boost::property_tree::ptree& pt);
	void json_nick_name_boost(const NickNameRecord& nickName, boost::property_tree::ptree& pt);
	void json_telephone_boost(const TelephoneRecord& telephones, boost::property_tree::ptree& pt);
	void json_emails_boost(const EmailRecord& emails, boost::property_tree::ptree& pt);
	void json_address_boost(const AddressRecord& addresses, boost::property_tree::ptree& pt);
	void json_event_boost(const EventRecord& event, boost::property_tree::ptree& pt);
	void json_work_info_boost(const WorkInfoRecord& work_info, boost::property_tree::ptree& pt);
	void json_url_boost(const UrlRecord& url_record, boost::property_tree::ptree& pt);
	void json_note_boost(const NoteRecord& note, boost::property_tree::ptree& pt);
	void json_social_net_boost(const SocialNetRecord& social_net, boost::property_tree::ptree& pt);
	void json_relation_boost(const RelationRecord& relation_record, boost::property_tree::ptree& pt);
public:
	void make_json(const std::vector<ContactData>& cards);
	inline const boost::property_tree::ptree& get_ptree() const {
		return card_tree;
	}
};




#endif // VCFE_JSON_MAKER_H