
#include <map>
#include <functional>
#include <vector>
#include <string>
#include "vcfe_serializer.hpp"

void BoostSerializer::serialize_phonetic_name(const PhoneticRecord& phoneticName, boost::property_tree::ptree& pt) {
	if (phoneticName.is_empty()) return;

	pt.put("PhoneticName.encoded", phoneticName.is_encoded());
	pt.put("PhoneticName.first", phoneticName.first);
	pt.put("PhoneticName.middle", phoneticName.middle);
	pt.put("PhoneticName.last", phoneticName.last);
}

void BoostSerializer::serialize_nick_name(const NickNameRecord& nickName, boost::property_tree::ptree& pt) {
	if (nickName.is_empty()) return;

	pt.put("NickNameRecord.encoded", nickName.is_encoded());
	pt.put("NickNameRecord.nick", nickName.nick);
}

void BoostSerializer::serialize_emails(const EmailRecord& emails, boost::property_tree::ptree& pt) {
	if (emails.is_empty()) return;

	pt.put("Email.encoded", emails.is_encoded());
	pt.put("Email.type", emails.type);
	pt.put("Email.address", emails.address);
	pt.put("Email.is_custom", emails.is_custom);
}

void BoostSerializer::serialize_address(const AddressRecord& addresses, boost::property_tree::ptree& pt) {
	if (addresses.is_empty()) return;

	pt.put("Address.encoded", addresses.is_encoded());
	pt.put("Address.type", addresses.type);
	pt.put("Address.street", addresses.street);
	pt.put("Address.city", addresses.city);
	pt.put("Address.region", addresses.region);
	pt.put("Address.index", addresses.index);
	pt.put("Address.country", addresses.country);
}

void BoostSerializer::serialize_event(const EventRecord& event, boost::property_tree::ptree& pt) {
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
	default: event_type_str = "NONE";
	}

	pt.put("Event.event_type", event_type_str);
}

void BoostSerializer::serialize_work_info(const WorkInfoRecord& work_info, boost::property_tree::ptree& pt) {
	if (work_info.is_empty()) return;

	pt.put("WorkInfo.encoded", work_info.is_encoded());
	pt.put("WorkInfo.company", work_info.company);
	pt.put("WorkInfo.department", work_info.department);
	pt.put("WorkInfo.title", work_info.title);
}

void BoostSerializer::serialize_url(const UrlRecord& url, boost::property_tree::ptree& pt) {
	if (url.is_empty()) return;

	pt.put("Url.encoded", url.is_encoded());
	pt.put("Url.url_address", url.url_address);
}

void BoostSerializer::serialize_note(const NoteRecord& note, boost::property_tree::ptree& pt) {
	if (note.is_empty()) return;

	pt.put("Note.encoded", note.is_encoded());
	pt.put("Note.note_text", note.note_text);
}

void BoostSerializer::serialize_social_net(const SocialNetRecord& social_net, boost::property_tree::ptree& pt) {
	if (social_net.is_empty()) return;

	pt.put("SocialNet.encoded", social_net.is_encoded());
	pt.put("SocialNet.name", social_net.name);
	pt.put("SocialNet.contact", social_net.contact);
	pt.put("SocialNet.is_custom", social_net.is_custom);
}

void BoostSerializer::serialize_relation(const RelationRecord& relation, boost::property_tree::ptree& pt) {
	if (relation.is_empty()) return;

	pt.put("Relation.encoded", relation.is_encoded());
	pt.put("Relation.name", relation.name);
	pt.put("Relation.type_name", relation.type_name);
	pt.put("Relation.type_num", relation.type_num);
}

/////////////////////////////////////////////////////////////////////////////////////////

#ifdef PUPPP
void BoostSerializer::serialize_vcards(const std::vector<ContactData>& cards) {
	for (size_t i = 0; i < cards.size(); ++i) {
		//std::ostringstream index_str;
		//index_str << std::setw(4) << std::setfill('0') << i;
		//index_str << std::setw(4) << std::setfill('0') << i;

		boost::property_tree::ptree card_node;
		const auto& card = cards[i];

		// Serialize if they are not empty
		if (!card.names.is_empty())			serialize_names(card.names, card_node);
		if (!card.phonetic_name.is_empty()) serialize_phonetic_name(card.phonetic_name, card_node);
		if (!card.nickname.is_empty())		serialize_nick_name(card.nickname, card_node);
		if (!card.workinfo.is_empty())		serialize_work_info(card.workinfo, card_node);
		if (!card.note.is_empty())			serialize_note(card.note, card_node);

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
		serialize_add_child(card.telephones, &BoostSerializer::serialize_telephone, "telephones");
		serialize_add_child(card.emails, &BoostSerializer::serialize_emails, "emails");
		serialize_add_child(card.addresses, &BoostSerializer::serialize_address, "addresses");
		serialize_add_child(card.urls, &BoostSerializer::serialize_url, "urls");
		serialize_add_child(card.events, &BoostSerializer::serialize_event, "events");
		serialize_add_child(card.socials, &BoostSerializer::serialize_social_net, "socials");
		serialize_add_child(card.relations, &BoostSerializer::serialize_relation, "relations");

		// Add the fully assembled card node to the main tree
		//card_tree.add_child("cards.Card_" + index_str.str(), card_node);
		card_tree.add_child("cards.Card", card_node);
	}
}


void BoostSerializer::serialize_names(const NameRecord& names, boost::property_tree::ptree& pt) {
	if (names.is_empty()) return;

	boost::property_tree::ptree& name_node = pt.add("Name", "");
	name_node.put("encoded", names.is_encoded());
	name_node.put("family", names.family);
	name_node.put("personal", names.personal);
	name_node.put("father", names.father);
	name_node.put("address_form", names.address_form);
	name_node.put("suffix", names.suffix);
}

void BoostSerializer::serialize_telephone(const TelephoneRecord& telephones, boost::property_tree::ptree& pt) {
	if (telephones.is_empty()) return;

	pt.put("Telephone.encoded", telephones.is_encoded());
	pt.put("Telephone.type", telephones.type);
	pt.put("Telephone.number", telephones.number);
	pt.put("Telephone.is_custom", telephones.is_custom);
}

#endif

/////////////////////////////////////////////////////////////////////////////////////////

void BoostSerializer::serialize_vcards(const std::vector<ContactData>& cards) {
	for (size_t i = 0; i < cards.size(); ++i) {

		boost::property_tree::ptree card_node;
		const auto& card = cards[i];

		// Serialize if names are not empty
		if (!card.names.is_empty())
			serialize_names(card.names, card_node);

		// Serialize telephones if not empty
		if (!card.telephones.empty()) {
			boost::property_tree::ptree telephones_node;
			for (const auto& telephone : card.telephones) {
				boost::property_tree::ptree telephone_node;
				serialize_telephone(telephone, telephone_node);
				telephones_node.push_back(std::make_pair("Telephone", telephone_node)); // Push Telephone node
			}
			card_node.add_child("telephones", telephones_node); // Add telephones node to card
		}

		card_tree.add_child("cards.Card", card_node); // Add card node to card tree
	}
}

void BoostSerializer::serialize_names(const NameRecord& names, boost::property_tree::ptree& pt) {
	if (names.is_empty()) return;

	boost::property_tree::ptree& name_node = pt.add_child("Name", {}); // Add Name node
	name_node.put("encoded", names.is_encoded());
	name_node.put("family", names.family);
	name_node.put("personal", names.personal);
	name_node.put("father", names.father);
	name_node.put("address_form", names.address_form);
	name_node.put("suffix", names.suffix);
}

void BoostSerializer::serialize_telephone(const TelephoneRecord& telephone, boost::property_tree::ptree& pt) {
	pt.put("encoded", telephone.is_encoded()); // Corrected attribute names
	pt.put("type", telephone.type);
	pt.put("number", telephone.number);
	pt.put("is_custom", telephone.is_custom);
}


/////////////////////////////////////////////////////////////////////////////////////////


void BoostSerializer::deserialize_name(NameRecord& names, const boost::property_tree::ptree& node) {
	if (node.empty()) return;
	if (node.get<bool>("encoded", false)) {
		names.set_encoded_state();
	}
	names.family = node.get<std::string>("family", "");
	names.personal = node.get<std::string>("personal", "");
	names.father = node.get<std::string>("father", "");
	names.address_form = node.get<std::string>("address_form", "");
	names.suffix = node.get<std::string>("suffix", "");
}

void BoostSerializer::deserialize_phonetic(PhoneticRecord& phonetic_names, const boost::property_tree::ptree& node) {
	if (node.empty()) return;
	if (node.get<bool>("encoded", false)) {
		phonetic_names.set_encoded_state();
	}
	phonetic_names.first = node.get<std::string>("first", "");
	phonetic_names.middle = node.get<std::string>("middle", "");
	phonetic_names.last = node.get<std::string>("last", "");
}

void BoostSerializer::deserialize_nick_name(NickNameRecord& nickname, const boost::property_tree::ptree& node) {
	if (node.empty()) return;
	if (node.get<bool>("encoded", false)) {
		nickname.set_encoded_state();
	}
	nickname.nick = node.get<std::string>("nick", "");
}

void BoostSerializer::deserialize_work_info(WorkInfoRecord& work_info, const boost::property_tree::ptree& node) {
	if (node.empty()) return;
	if (node.get<bool>("encoded", false)) {
		work_info.set_encoded_state();
	}
	work_info.company = node.get<std::string>("company", "");
	work_info.department = node.get<std::string>("department", "");
	work_info.title = node.get<std::string>("title", "");
}

void BoostSerializer::deserialize_note(NoteRecord& note, const boost::property_tree::ptree& node) {
	if (node.empty()) return;
	if (node.get<bool>("encoded", false)) {
		note.set_encoded_state();
	}
	note.note_text = node.get<std::string>("note_text", "");
}

void BoostSerializer::deserialize_telephone(TelephoneRecord& telephones, const boost::property_tree::ptree& node) {
	if (node.empty()) return;

	if (node.get<bool>("Telephone.encoded", false)) {
		telephones.set_encoded_state();
	}
	telephones.type = node.get<std::string>("Telephone.type", "");
	telephones.number = node.get<std::string>("Telephone.number", "");
	telephones.is_custom = node.get<bool>("Telephone.is_custom", false);
}

void BoostSerializer::deserialize_email(EmailRecord& emails, const boost::property_tree::ptree& node) {
	if (node.empty()) return;

	if (node.get<bool>("Email.encoded", false)) {
		emails.set_encoded_state();
	}
	emails.type = node.get<std::string>("Email.type", "");
	emails.address = node.get<std::string>("Email.address", "");
	emails.is_custom = node.get<bool>("Email.is_custom", false);
}

void BoostSerializer::deserialize_address(AddressRecord& addresses, const boost::property_tree::ptree& node) {
	if (node.empty()) return;

	if (node.get<bool>("Address.encoded", false)) {
		addresses.set_encoded_state();
	}
	addresses.type = node.get<std::string>("Address.type", "");
	addresses.street = node.get<std::string>("Address.street", "");
	addresses.city = node.get<std::string>("Address.city", "");
	addresses.region = node.get<std::string>("Address.region", "");
	addresses.index = node.get<std::string>("Address.index", "");
	addresses.country = node.get<std::string>("Address.country", "");
}

void BoostSerializer::deserialize_url(UrlRecord& url, const boost::property_tree::ptree& node) {
	if (node.empty()) return;

	if (node.get<bool>("Url.encoded", false)) {
		url.set_encoded_state();
	}
	url.url_address = node.get<std::string>("Url.url_address", "");
}

void BoostSerializer::deserialize_event(EventRecord& event, const boost::property_tree::ptree& node) {
	if (node.empty()) return;

	if (node.get<bool>("Event.encoded", false)) {
		event.set_encoded_state();
	}
	event.event_name = node.get<std::string>("Event.event_name", "");
	event.day = node.get<std::string>("Event.day", "");
	event.month = node.get<std::string>("Event.month", "");
	event.year = node.get<std::string>("Event.year", "");

	// Convert the EventType string back to enum
	std::string event_type_str = node.get<std::string>("Event.event_type", "");
	if (event_type_str == "SPECIAL") {
		event.event_type = EventType::SPECIAL;
	}
	else if (event_type_str == "ANNIVERSARY") {
		event.event_type = EventType::ANNIVERSARY;
	}
	else if (event_type_str == "OTHER") {
		event.event_type = EventType::OTHER;
	}
	else if (event_type_str == "BDAY") {
		event.event_type = EventType::BDAY;
	}
	else if (event_type_str == "ENCODED") {
		event.event_type = EventType::ENCODED;
	}
	else {
		event.event_type = EventType::NONE;
	}
}

void BoostSerializer::deserialize_social_net(SocialNetRecord& social_net, const boost::property_tree::ptree& node) {
	if (node.empty()) return;

	if (node.get<bool>("SocialNet.encoded", false)) {
		social_net.set_encoded_state();
	}
	social_net.name = node.get<std::string>("SocialNet.name", "");
	social_net.contact = node.get<std::string>("SocialNet.contact", "");
	social_net.is_custom = node.get<bool>("SocialNet.is_custom", false);
}

void BoostSerializer::deserialize_relation(RelationRecord& relation, const boost::property_tree::ptree& node) {
	if (node.empty()) return;

	if (node.get<bool>("Relation.encoded", false)) {
		relation.set_encoded_state();
	}
	relation.name = node.get<std::string>("Relation.name", "");
	relation.type_name = node.get<std::string>("Relation.type_name", "");
	relation.type_num = node.get<int>("Relation.type_num", 0);
}


#ifdef DDDD

#endif
