#include <iomanip>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "vcfe_data_containers.h"
#include "vcfe_json_maker.h"

void JsonMakerBoost::make_json(const std::vector<ContactData>& cards) {
	for (size_t i = 0; i < cards.size(); ++i) {
		std::ostringstream index_str;
		index_str << std::setw(4) << std::setfill('0') << i;

		boost::property_tree::ptree card_node;
		const auto& card = cards[i];

		// Serialize and add names, phonetic names, and nicknames if they are not empty
		if (!card.names.is_empty()) json_names_boost(card.names, card_node);
		if (!card.phonetic_name.is_empty()) json_phonetic_name_boost(card.phonetic_name, card_node);
		if (!card.nickname.is_empty()) json_nick_name_boost(card.nickname, card_node);

		// Serialize work info and note if not empty
		if (!card.workinfo.is_empty()) json_work_info_boost(card.workinfo, card_node);
		if (!card.note.is_empty()) json_note_boost(card.note, card_node);

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
		serialize_add_child(card.telephones, &JsonMakerBoost::json_telephone_boost, "telephones");
		serialize_add_child(card.emails, &JsonMakerBoost::json_emails_boost, "emails");
		serialize_add_child(card.addresses, &JsonMakerBoost::json_address_boost, "addresses");
		serialize_add_child(card.urls, &JsonMakerBoost::json_url_boost, "urls");
		serialize_add_child(card.events, &JsonMakerBoost::json_event_boost, "events");
		serialize_add_child(card.socials, &JsonMakerBoost::json_social_net_boost, "socials");
		serialize_add_child(card.relations, &JsonMakerBoost::json_relation_boost, "relations");

		// Add the fully assembled card node to the main tree
		card_tree.add_child("cards.Card_" + index_str.str(), card_node);
	}
}

void JsonMakerBoost::json_names_boost(const NameRecord& names, boost::property_tree::ptree& pt) {
	if (names.is_empty()) return;

	pt.put("Name.encoded", names.is_encoded());
	pt.put("Name.family", names.family);
	pt.put("Name.personal", names.personal);
	pt.put("Name.father", names.father);
	pt.put("Name.address_form", names.address_form);
	pt.put("Name.suffix", names.suffix);
}

void JsonMakerBoost::json_phonetic_name_boost(const PhoneticRecord& phoneticName, boost::property_tree::ptree& pt) {
	if (phoneticName.is_empty()) return;

	pt.put("PhoneticName.encoded", phoneticName.is_encoded());
	pt.put("PhoneticName.first", phoneticName.first);
	pt.put("PhoneticName.middle", phoneticName.middle);
	pt.put("PhoneticName.last", phoneticName.last);
}

void JsonMakerBoost::json_nick_name_boost(const NickNameRecord& nickName, boost::property_tree::ptree& pt) {
	if (nickName.is_empty()) return;

	pt.put("NickNameRecord.encoded", nickName.is_encoded());
	pt.put("NickNameRecord.nick", nickName.nick);
}

void JsonMakerBoost::json_telephone_boost(const TelephoneRecord& telephones, boost::property_tree::ptree& pt) {
	if (telephones.is_empty()) return;

	pt.put("Telephone.encoded", telephones.is_encoded());
	pt.put("Telephone.type", telephones.type);
	pt.put("Telephone.number", telephones.number);
	pt.put("Telephone.is_custom", telephones.is_custom);
}

void JsonMakerBoost::json_emails_boost(const EmailRecord& emails, boost::property_tree::ptree& pt) {
	if (emails.is_empty()) return;

	pt.put("Email.encoded", emails.is_encoded());
	pt.put("Email.type", emails.type);
	pt.put("Email.address", emails.address);
	pt.put("Email.is_custom", emails.is_custom);
}

void JsonMakerBoost::json_address_boost(const AddressRecord& addresses, boost::property_tree::ptree& pt) {
	if (addresses.is_empty()) return;

	pt.put("Address.encoded", addresses.is_encoded());
	pt.put("Address.type", addresses.type);
	pt.put("Address.street", addresses.street);
	pt.put("Address.city", addresses.city);
	pt.put("Address.region", addresses.region);
	pt.put("Address.index", addresses.index);
	pt.put("Address.country", addresses.country);
}

void JsonMakerBoost::json_event_boost(const EventRecord& event, boost::property_tree::ptree& pt) {
	if (event.is_empty()) return;

	pt.put("Event.encoded", event.is_encoded());
	pt.put("Event.event_name", event.event_name);
	pt.put("Event.day", event.day);
	pt.put("Event.month", event.month);
	pt.put("Event.year", event.year);

	// Convert the EventType enum to a string for serialization
	std::string event_type_str;
	switch (event.event_type) {
	case EventType::SPECIAL: event_type_str = "SPECIAL"; break;
	case EventType::ANNIVERSARY: event_type_str = "ANNIVERSARY"; break;
	case EventType::OTHER: event_type_str = "OTHER"; break;
	case EventType::BDAY: event_type_str = "BDAY"; break;
	case EventType::ENCODED: event_type_str = "ENCODED"; break;
	default: event_type_str = "UNKNOWN";
	}

	pt.put("Event.event_type", event_type_str);
}

void JsonMakerBoost::json_work_info_boost(const WorkInfoRecord& work_info, boost::property_tree::ptree& pt) {
	if (work_info.is_empty()) return;

	pt.put("WorkInfo.encoded", work_info.is_encoded());
	pt.put("WorkInfo.company", work_info.company);
	pt.put("WorkInfo.department", work_info.department);
	pt.put("WorkInfo.title", work_info.title);
}

void JsonMakerBoost::json_url_boost(const UrlRecord& url_record, boost::property_tree::ptree& pt) {
	if (url_record.is_empty()) return;

	pt.put("Url.encoded", url_record.is_encoded());
	pt.put("Url.url_address", url_record.url_address);
}

void JsonMakerBoost::json_note_boost(const NoteRecord& note, boost::property_tree::ptree& pt) {
	if (note.is_empty()) return;

	pt.put("Note.encoded", note.is_encoded());
	pt.put("Note.note_text", note.note_text);
}

void JsonMakerBoost::json_social_net_boost(const SocialNetRecord& social_net, boost::property_tree::ptree& pt) {
	if (social_net.is_empty()) return;

	pt.put("SocialNet.encoded", social_net.is_encoded());
	pt.put("SocialNet.name", social_net.name);
	pt.put("SocialNet.contact", social_net.contact);
	pt.put("SocialNet.is_custom", social_net.is_custom);
}

void JsonMakerBoost::json_relation_boost(const RelationRecord& relation_record, boost::property_tree::ptree& pt) {
	if (relation_record.is_empty()) return;

	pt.put("Relation.encoded", relation_record.is_encoded());
	pt.put("Relation.name", relation_record.name);
	pt.put("Relation.type_name", relation_record.type_name);
	pt.put("Relation.type_num", relation_record.type_num);
}