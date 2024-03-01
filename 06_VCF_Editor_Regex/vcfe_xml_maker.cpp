
#include <iomanip>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "vcfe_data_containers.h"
#include "vcfe_xml_maker.h"

void XmlMakerBoost::make_xml(const std::vector<ContactData>& cards) {
	for (size_t i = 0; i < cards.size(); ++i) {
		std::ostringstream index_str;
		index_str << std::setw(4) << std::setfill('0') << i;

		boost::property_tree::ptree card_node;
		const auto& card = cards[i];

		// Serialize and add names, phonetic names, and nicknames if they are not empty
		if (!card.names.is_empty()) xml_names_boost(card.names, card_node);
		if (!card.phonetic_name.is_empty()) xml_phonetic_name_boost(card.phonetic_name, card_node);
		if (!card.nickname.is_empty()) xml_nick_name_boost(card.nickname, card_node);

		// Serialize work info and note if not empty
		if (!card.workinfo.is_empty()) xml_work_info_boost(card.workinfo, card_node);
		if (!card.note.is_empty()) xml_note_boost(card.note, card_node);

		// Helper function to serialize and add child nodes
		auto serialize_add_child = [&](const auto& collection, auto serializer, const std::string& child_name) {
			if (!collection.empty()) {
				boost::property_tree::ptree intermediate_node;
				for (const auto& item : collection) {
					if (!item.is_empty()) {
						boost::property_tree::ptree item_node;
						(this->*serializer)(item, item_node);
						intermediate_node.push_back(std::make_pair("", item_node));
					}
				}
				if (!intermediate_node.empty()) {
					card_node.add_child(child_name, intermediate_node);
				}
			}
			};

		// Serialize other attributes using the helper
		serialize_add_child(card.telephones, &XmlMakerBoost::xml_telephone_boost, "telephones");
		serialize_add_child(card.emails, &XmlMakerBoost::xml_emails_boost, "emails");
		serialize_add_child(card.addresses, &XmlMakerBoost::xml_address_boost, "addresses");
		serialize_add_child(card.urls, &XmlMakerBoost::xml_url_boost, "urls");
		serialize_add_child(card.events, &XmlMakerBoost::xml_event_boost, "events");
		serialize_add_child(card.socials, &XmlMakerBoost::xml_social_net_boost, "socials");
		serialize_add_child(card.relations, &XmlMakerBoost::xml_relation_boost, "relations");

		// Add the fully assembled card node to the main tree with a unique identifier
		card_tree.add_child("cards.Card_" + index_str.str(), card_node);
	}
}

void XmlMakerBoost::xml_names_boost(const NameRecord& names, boost::property_tree::ptree& pt) {
	if (names.is_empty()) return;

	boost::property_tree::ptree& name_node = pt.add("Name", "");
	name_node.put("encoded",	  names.is_encoded());
	name_node.put("family",		  names.family);
	name_node.put("personal",	  names.personal);
	name_node.put("father",		  names.father);
	name_node.put("address_form", names.address_form);
	name_node.put("suffix",		  names.suffix);
}

void XmlMakerBoost::xml_phonetic_name_boost(const PhoneticRecord& phoneticName, boost::property_tree::ptree& pt) {
    if (phoneticName.is_empty()) return;

    boost::property_tree::ptree& phonetic_node = pt.add("PhoneticName", "");
    phonetic_node.put("encoded", phoneticName.is_encoded());
    phonetic_node.put("first",   phoneticName.first);
    phonetic_node.put("middle",  phoneticName.middle);
    phonetic_node.put("last",    phoneticName.last);
}

void XmlMakerBoost::xml_nick_name_boost(const NickNameRecord& nickName, boost::property_tree::ptree& pt) {
    if (nickName.is_empty()) return;

    boost::property_tree::ptree& nick_name_node = pt.add("NickNameRecord", "");
    nick_name_node.put("encoded", nickName.is_encoded());
    nick_name_node.put("nick",    nickName.nick);
}

void XmlMakerBoost::xml_telephone_boost(const TelephoneRecord& telephones, boost::property_tree::ptree& pt) {
	if (telephones.is_empty()) return;

	boost::property_tree::ptree& telephone_node = pt.add("Telephone", "");
	telephone_node.put("encoded", telephones.is_encoded());
	telephone_node.put("type", telephones.type);
	telephone_node.put("number", telephones.number);
	telephone_node.put("is_custom", telephones.is_custom);
}

void XmlMakerBoost::xml_emails_boost(const EmailRecord& emails, boost::property_tree::ptree& pt) {
	if (emails.is_empty()) return;

	boost::property_tree::ptree email_node;
	email_node.put("encoded", emails.is_encoded());
	email_node.put("type", emails.type);
	email_node.put("address", emails.address);
	email_node.put("is_custom", emails.is_custom);
	pt.add_child("Email", email_node);
}

void XmlMakerBoost::xml_address_boost(const AddressRecord& addresses, boost::property_tree::ptree& pt) {
	if (addresses.is_empty()) return;

	boost::property_tree::ptree address_node;
	address_node.put("encoded", addresses.is_encoded());
	address_node.put("type",	addresses.type);
	address_node.put("street",  addresses.street);
	address_node.put("city",	addresses.city);
	address_node.put("region",  addresses.region);
	address_node.put("index",	addresses.index);
	address_node.put("country", addresses.country);
	pt.add_child("Address",		address_node);
}

void XmlMakerBoost::xml_event_boost(const EventRecord& event, boost::property_tree::ptree& pt) {
	if (event.is_empty()) return;

	boost::property_tree::ptree event_node;
	event_node.put("encoded",	 event.is_encoded());
	event_node.put("event_name", event.event_name);
	event_node.put("day",		 event.day);
	event_node.put("month",		 event.month);
	event_node.put("year",		 event.year);

	std::string event_type_str;
	switch (event.event_type) {
	case EventType::SPECIAL:	 event_type_str = "SPECIAL";	 break;
	case EventType::ANNIVERSARY: event_type_str = "ANNIVERSARY"; break;
	case EventType::OTHER:		 event_type_str = "OTHER";		 break;
	case EventType::BDAY:		 event_type_str = "BDAY";		 break;
	case EventType::ENCODED:	 event_type_str = "ENCODED";	 break;
	default: event_type_str = "UNKNOWN";
	}

	event_node.put("event_type", event_type_str);
	pt.add_child("Event", event_node);
}

void XmlMakerBoost::xml_work_info_boost(const WorkInfoRecord& work_info, boost::property_tree::ptree& pt) {
	if (work_info.is_empty()) return;

	boost::property_tree::ptree work_info_node;
	work_info_node.put("encoded", work_info.is_encoded());
	work_info_node.put("company", work_info.company);
	work_info_node.put("department", work_info.department);
	work_info_node.put("title", work_info.title);

	pt.add_child("WorkInfo", work_info_node);
}

void XmlMakerBoost::xml_url_boost(const UrlRecord& url_record, boost::property_tree::ptree& pt) {
	if (url_record.is_empty()) return;

	boost::property_tree::ptree url_node;
	url_node.put("encoded", url_record.is_encoded());
	url_node.put("url_address", url_record.url_address);

	pt.add_child("Url", url_node);
}

void XmlMakerBoost::xml_note_boost(const NoteRecord& note, boost::property_tree::ptree& pt) {
	if (note.is_empty()) return;

	boost::property_tree::ptree note_node;
	note_node.put("encoded", note.is_encoded());
	note_node.put("note_text", note.note_text);

	pt.add_child("Note", note_node);
}

void XmlMakerBoost::xml_social_net_boost(const SocialNetRecord& social_net, boost::property_tree::ptree& pt) {
	if (social_net.is_empty()) return;

	boost::property_tree::ptree social_net_node;
	social_net_node.put("encoded", social_net.is_encoded());
	social_net_node.put("name", social_net.name);
	social_net_node.put("contact", social_net.contact);
	social_net_node.put("is_custom", social_net.is_custom);

	pt.add_child("SocialNet", social_net_node);
}

void XmlMakerBoost::xml_relation_boost(const RelationRecord& relation_record, boost::property_tree::ptree& pt) {
	if (relation_record.is_empty()) return;

	boost::property_tree::ptree relation_node;
	relation_node.put("encoded", relation_record.is_encoded());
	relation_node.put("name", relation_record.name);
	relation_node.put("type_name", relation_record.type_name);
	relation_node.put("type_num", relation_record.type_num);

	pt.add_child("Relation", relation_node);
}