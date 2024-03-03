#include <map>
#include <functional>
#include <vector>
#include <string>
#include "vcfe_ptree_parser.hpp"

void PropertyTreeSerializer::serialize_vcards(const std::vector<ContactData>& cards) {
	
	if (was_new_file_handled == true) return;

	was_new_file_handled = true;
	
	for (size_t i = 0; i < cards.size(); ++i) {
		boost::property_tree::ptree card_node;
		const auto& card = cards[i];

		// Serialize single entries
		if (!card.names.is_empty())         serialize_names(card.names, card_node);
		if (!card.phonetic_name.is_empty()) serialize_phonetic_name(card.phonetic_name, card_node);
		if (!card.nickname.is_empty())      serialize_nick_name(card.nickname, card_node);
		if (!card.workinfo.is_empty())      serialize_work_info(card.workinfo, card_node);
		if (!card.note.is_empty())          serialize_note(card.note, card_node);

		// Serialize arrays
		if (!card.telephones.empty()) {
			boost::property_tree::ptree node;
			for (const auto& telephone : card.telephones) {
				boost::property_tree::ptree telephone_node;
				serialize_telephone(telephone, telephone_node);
				node.push_back(std::make_pair("Telephone", telephone_node));
			}
			card_node.add_child("telephones", node); // Add telephones node to card
		}

		if (!card.emails.empty()) {
			boost::property_tree::ptree node;
			for (const auto& email : card.emails) {
				boost::property_tree::ptree email_node;
				serialize_emails(email, email_node);
				node.push_back(std::make_pair("Email", email_node));
			}
			card_node.add_child("emails", node); // Add emails node to card
		}

		if (!card.addresses.empty()) {
			boost::property_tree::ptree node;
			for (const auto& address : card.addresses) {
				boost::property_tree::ptree address_node;
				serialize_address(address, address_node);
				node.push_back(std::make_pair("Address", address_node));
			}
			card_node.add_child("addresses", node); // Add addresses node to card
		}

		if (!card.events.empty()) {
			boost::property_tree::ptree node;
			for (const auto& event : card.events) {
				boost::property_tree::ptree event_node;
				serialize_event(event, event_node);
				node.push_back(std::make_pair("Event", event_node));
			}
			card_node.add_child("events", node); // Add events node to card
		}

		if (!card.urls.empty()) {
			boost::property_tree::ptree node;
			for (const auto& url : card.urls) {
				boost::property_tree::ptree url_node;
				serialize_url(url, url_node);
				node.push_back(std::make_pair("Url", url_node));
			}
			card_node.add_child("urls", node); // Add urls node to card
		}

		if (!card.socials.empty()) {
			boost::property_tree::ptree node;
			for (const auto& social : card.socials) {
				boost::property_tree::ptree social_node;
				serialize_social_net(social, social_node);
				node.push_back(std::make_pair("Social", social_node));
			}
			card_node.add_child("social_net", node); // Add social_net node to card
		}
		if (!card.relations.empty()) {
			boost::property_tree::ptree node;
			for (const auto& relation : card.relations) {
				boost::property_tree::ptree relation_node;
				serialize_relation(relation, relation_node);
				node.push_back(std::make_pair("Relation", relation_node));
			}
			card_node.add_child("relations", node); // Add relations node to card
		}

		card_tree.add_child("cards.Card", card_node); // Add card node to card tree
	}
}

void PropertyTreeSerializer::serialize_names(const NameRecord& names, boost::property_tree::ptree& pt) {
	if (names.is_empty()) return;

	boost::property_tree::ptree& name_node = pt.add_child("Name", {}); // Add Name node
	name_node.put("encoded", names.is_encoded());
	name_node.put("family", names.family);
	name_node.put("personal", names.personal);
	name_node.put("father", names.father);
	name_node.put("address_form", names.address_form);
	name_node.put("suffix", names.suffix);
}

void PropertyTreeSerializer::serialize_phonetic_name(const PhoneticRecord& phoneticName, boost::property_tree::ptree& pt) {
	if (phoneticName.is_empty()) return;

	pt.put("PhoneticName.encoded", phoneticName.is_encoded());
	pt.put("PhoneticName.first", phoneticName.first);
	pt.put("PhoneticName.middle", phoneticName.middle);
	pt.put("PhoneticName.last", phoneticName.last);
}

void PropertyTreeSerializer::serialize_nick_name(const NickNameRecord& nickName, boost::property_tree::ptree& pt) {
	if (nickName.is_empty()) return;

	pt.put("NickNameRecord.encoded", nickName.is_encoded());
	pt.put("NickNameRecord.nick", nickName.nick);
}

void PropertyTreeSerializer::serialize_work_info(const WorkInfoRecord& work_info, boost::property_tree::ptree& pt) {
	if (work_info.is_empty()) return;

	pt.put("WorkInfo.encoded", work_info.is_encoded());
	pt.put("WorkInfo.company", work_info.company);
	pt.put("WorkInfo.department", work_info.department);
	pt.put("WorkInfo.title", work_info.title);
}

void PropertyTreeSerializer::serialize_note(const NoteRecord& note, boost::property_tree::ptree& pt) {
	if (note.is_empty()) return;

	pt.put("Note.encoded", note.is_encoded());
	pt.put("Note.note_text", note.note_text);
}

void PropertyTreeSerializer::serialize_telephone(const TelephoneRecord& telephone, boost::property_tree::ptree& pt) {
	if (telephone.is_empty()) return;

	pt.put("encoded", telephone.is_encoded());
	pt.put("type", telephone.type);
	pt.put("number", telephone.number);
	pt.put("is_custom", telephone.is_custom);
}

void PropertyTreeSerializer::serialize_emails(const EmailRecord& emails, boost::property_tree::ptree& pt) {
	if (emails.is_empty()) return;

	pt.put("encoded", emails.is_encoded());
	pt.put("type", emails.type);
	pt.put("address", emails.address);
	pt.put("is_custom", emails.is_custom);
}

void PropertyTreeSerializer::serialize_address(const AddressRecord& addresses, boost::property_tree::ptree& pt) {
	if (addresses.is_empty()) return;

	pt.put("encoded", addresses.is_encoded());
	pt.put("type", addresses.type);
	pt.put("street", addresses.street);
	pt.put("city", addresses.city);
	pt.put("region", addresses.region);
	pt.put("index", addresses.index);
	pt.put("country", addresses.country);
}

void PropertyTreeSerializer::serialize_event(const EventRecord& event, boost::property_tree::ptree& pt) {
	if (event.is_empty()) return;

	pt.put("encoded", event.is_encoded());
	pt.put("event_name", event.event_name);
	pt.put("day", event.day);
	pt.put("month", event.month);
	pt.put("year", event.year);

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

	pt.put("event_type", event_type_str);
}

void PropertyTreeSerializer::serialize_url(const UrlRecord& url, boost::property_tree::ptree& pt) {
	if (url.is_empty()) return;

	pt.put("encoded",	  url.is_encoded());
	pt.put("url_address", url.url_address);
}

void PropertyTreeSerializer::serialize_social_net(const SocialNetRecord& social_net, boost::property_tree::ptree& pt) {
	if (social_net.is_empty()) return;

	pt.put("encoded",	social_net.is_encoded());
	pt.put("name",		social_net.name);
	pt.put("contact",	social_net.contact);
	pt.put("is_custom", social_net.is_custom);
}

void PropertyTreeSerializer::serialize_relation(const RelationRecord& relation, boost::property_tree::ptree& pt) {
	if (relation.is_empty()) return;

	pt.put("encoded",	relation.is_encoded());
	pt.put("name",		relation.name);
	pt.put("type_name", relation.type_name);
	pt.put("type_num",  relation.type_num);
}

/////////////////////////////////////////////////////////////////////////////////////////

#ifdef NOUSE
#endif

/////////////////////////////////////////////////////////////////////////////////////////


void PropertyTreeSerializer::deserialize_vcards(const std::string& filename, std::vector<ContactData>& cards) {
	try {
		// Determine file extension
		std::string file_extension = filename.substr(filename.find_last_of(".") + 1);

		// Load the file into the property tree based on the file extension
		if (file_extension == "json") {
			boost::property_tree::read_json(filename, card_tree);
		}
		else if (file_extension == "xml") {
			boost::property_tree::read_xml(filename, card_tree);
		}
		else {
			// Unsupported file type, handle appropriately
			throw std::invalid_argument("Unsupported file type");
		}

		for (const auto& card_node : card_tree.get_child("cards")) {
			ContactData card;

			// Deserialize names
			if (auto names_node = card_node.second.get_child_optional("Name")) {
				deserialize_names(names_node.get(), card.names);
			}

			// Deserialize phonetic names
			if (auto phonetic_names_node = card_node.second.get_child_optional("PhoneticName")) {
				deserialize_phonetic(phonetic_names_node.get(), card.phonetic_name);
			}

			// Deserialize nicknames
			if (auto nickname_node = card_node.second.get_child_optional("NickName")) {
				deserialize_nick_name(nickname_node.get(), card.nickname);
			}

			// Deserialize work info
			if (auto work_info_node = card_node.second.get_child_optional("WorkInfo")) {
				deserialize_work_info(work_info_node.get(), card.workinfo);
			}

			// Deserialize notes
			if (auto note_node = card_node.second.get_child_optional("Note")) {
				deserialize_note(note_node.get(), card.note);
			}

			// Deserialize telephones
			if (auto telephones_node = card_node.second.get_child_optional("telephones")) {
				for (const auto& telephone_node : telephones_node.get()) {
					TelephoneRecord telephone;
					deserialize_telephone(telephone_node.second, telephone);
					card.telephones.push_back(telephone);
				}
			}

			// Deserialize emails
			if (auto emails_node = card_node.second.get_child_optional("emails")) {
				for (const auto& email_node : emails_node.get()) {
					EmailRecord email;
					deserialize_email(email_node.second, email);
					card.emails.push_back(email);
				}
			}

			// Deserialize addresses
			if (auto addresses_node = card_node.second.get_child_optional("addresses")) {
				for (const auto& address_node : addresses_node.get()) {
					AddressRecord address;
					deserialize_address(address_node.second, address);
					card.addresses.push_back(address);
				}
			}

			// Deserialize URLs
			if (auto urls_node = card_node.second.get_child_optional("urls")) {
				for (const auto& url_node : urls_node.get()) {
					UrlRecord url;
					deserialize_url(url_node.second, url);
					card.urls.push_back(url);
				}
			}

			// Deserialize events
			if (auto events_node = card_node.second.get_child_optional("events")) {
				for (const auto& event_node : events_node.get()) {
					EventRecord event;
					deserialize_event(event_node.second, event);
					card.events.push_back(event);
				}
			}

			// Deserialize social networks
			if (auto social_nets_node = card_node.second.get_child_optional("social_nets")) {
				for (const auto& social_net_node : social_nets_node.get()) {
					SocialNetRecord social_net;
					deserialize_social_net(social_net_node.second, social_net);
					card.socials.push_back(social_net);
				}
			}

			// Deserialize relations
			if (auto relations_node = card_node.second.get_child_optional("relations")) {
				for (const auto& relation_node : relations_node.get()) {
					RelationRecord relation;
					deserialize_relation(relation_node.second, relation);
					card.relations.push_back(relation);
				}
			}

			cards.push_back(card);
		}
	}
	catch (const boost::property_tree::json_parser_error& e) {
		std::cerr << "JSON parser error: " << e.what() << '\n';
		std::cerr << "Line: " << e.line() << '\n';
		std::cerr << "Filename: " << e.filename() << '\n';
	}
	catch (const boost::property_tree::xml_parser_error& e) {
		std::cerr << "XML parser error: " << e.what() << '\n';
		std::cerr << "Line: " << e.line() << '\n';
		std::cerr << "Filename: " << e.filename() << '\n';
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Invalid argument: " << e.what() << '\n';
	}
	catch (const std::exception& e) {
		std::cerr << "An error occurred: " << e.what() << '\n';
	}
}


void PropertyTreeSerializer::deserialize_names(const boost::property_tree::ptree& pt, NameRecord& names) {
	if (pt.empty()) return;

	if (pt.get<bool>("encoded", false)) {
		names.set_encoded_state();
	}
	names.family = pt.get<std::string>("family");
	names.personal = pt.get<std::string>("personal");
	names.father = pt.get<std::string>("father");
	names.address_form = pt.get<std::string>("address_form");
	names.suffix = pt.get<std::string>("suffix");
}

void PropertyTreeSerializer::deserialize_phonetic(const boost::property_tree::ptree& node, PhoneticRecord& phonetic_names) {
	if (node.empty()) return;
	if (node.get<bool>("encoded", false)) {
		phonetic_names.set_encoded_state();
	}
	phonetic_names.first = node.get<std::string>("first", "");
	phonetic_names.middle = node.get<std::string>("middle", "");
	phonetic_names.last = node.get<std::string>("last", "");
}

void PropertyTreeSerializer::deserialize_nick_name(const boost::property_tree::ptree& node, NickNameRecord& nickname) {
	if (node.empty()) return;
	if (node.get<bool>("encoded", false)) {
		nickname.set_encoded_state();
	}
	nickname.nick = node.get<std::string>("nick", "");
}

void PropertyTreeSerializer::deserialize_work_info(const boost::property_tree::ptree& node, WorkInfoRecord& work_info) {
	if (node.empty()) return;
	if (node.get<bool>("encoded", false)) {
		work_info.set_encoded_state();
	}
	work_info.company = node.get<std::string>("company", "");
	work_info.department = node.get<std::string>("department", "");
	work_info.title = node.get<std::string>("title", "");
}

void PropertyTreeSerializer::deserialize_note(const boost::property_tree::ptree& node, NoteRecord& note) {
	if (node.empty()) return;
	if (node.get<bool>("encoded", false)) {
		note.set_encoded_state();
	}
	note.note_text = node.get<std::string>("note_text", "");
}

void PropertyTreeSerializer::deserialize_telephone(const boost::property_tree::ptree& pt, TelephoneRecord& telephone) {
	if (pt.empty()) return;

	if (pt.get<bool>("encoded", false)) {
		telephone.set_encoded_state();
	}

	telephone.type = pt.get<std::string>("type");
	telephone.number = pt.get<std::string>("number");
	telephone.is_custom = pt.get<bool>("is_custom");
}

void PropertyTreeSerializer::deserialize_email(const boost::property_tree::ptree& pt, EmailRecord& email) {
	if (pt.empty()) return;

	if (pt.get<bool>("encoded", false)) {
		email.set_encoded_state();
	}
	email.type = pt.get<std::string>("type", "");
	email.address = pt.get<std::string>("address", "");
	email.is_custom = pt.get<bool>("is_custom", false);
}

void PropertyTreeSerializer::deserialize_address(const boost::property_tree::ptree& pt, AddressRecord& address) {
	if (pt.empty()) return;

	if (pt.get<bool>("encoded", false)) {
		address.set_encoded_state();
	}
	address.type = pt.get<std::string>("type", "");
	address.street = pt.get<std::string>("street", "");
	address.city = pt.get<std::string>("city", "");
	address.region = pt.get<std::string>("region", "");
	address.index = pt.get<std::string>("index", "");
	address.country = pt.get<std::string>("country", "");
}

void PropertyTreeSerializer::deserialize_url(const boost::property_tree::ptree& pt, UrlRecord& url) {
	if (pt.empty()) return;

	if (pt.get<bool>("encoded", false)) {
		url.set_encoded_state();
	}
	url.url_address = pt.get<std::string>("url_address", "");
}

void PropertyTreeSerializer::deserialize_event(const boost::property_tree::ptree& node, EventRecord& event) {
	if (node.empty()) return;

	if (node.get<bool>("encoded", false)) {
		event.set_encoded_state();
	}
	event.event_name = node.get<std::string>("event_name", "");
	event.day = node.get<std::string>("day", "");
	event.month = node.get<std::string>("month", "");
	event.year = node.get<std::string>("year", "");

	// Convert the EventType string back to enum
	std::string event_type_str = node.get<std::string>("event_type", "");
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

void PropertyTreeSerializer::deserialize_social_net(const boost::property_tree::ptree& node, SocialNetRecord& social_net) {
	if (node.empty()) return;

	if (node.get<bool>("encoded", false)) {
		social_net.set_encoded_state();
	}
	social_net.name = node.get<std::string>("name", "");
	social_net.contact = node.get<std::string>("contact", "");
	social_net.is_custom = node.get<bool>("is_custom", false);
}

void PropertyTreeSerializer::deserialize_relation(const boost::property_tree::ptree& node, RelationRecord& relation) {
	if (node.empty()) return;

	if (node.get<bool>("encoded", false)) {
		relation.set_encoded_state();
	}
	relation.name = node.get<std::string>("name", "");
	relation.type_name = node.get<std::string>("type_name", "");
	relation.type_num = node.get<int>("type_num", 0);
}

#ifdef DDDD

#endif