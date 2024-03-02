#define NOUSE
#ifdef NOUSE

#include <iostream>
#include <string>

#include "vcfe_data_containers.h"
#include "vcfe_vcf_parser.h"
#include "vcfe_vcf_printer.h"
#include "vcfe_json_maker.h"
#include "vcfe_xml_maker.h"

/*
*/
////////////////
// 

#ifdef OOOOOO
void BoostSerializer::deserialize_vcards(const std::string& filename, std::vector<ContactData>& cards) {
	try {
		// Determine file extension
		std::string file_extension = filename.substr(filename.find_last_of(".") + 1);

		// Create an empty property tree
		boost::property_tree::ptree pt;

		// Load the file into the property tree based on the file extension
		if (file_extension == "json") {
			boost::property_tree::read_json(filename, pt);
		}
		else if (file_extension == "xml") {
			boost::property_tree::read_xml(filename, pt);
		}
		else {
			// Unsupported file type, handle appropriately
			throw std::invalid_argument("Unsupported file type");
		}

		// Iterate over each "cards.Card" node in the property tree
		for (const auto& card_node : pt.get_child("cards")) {
			// Create a ContactData object to store the deserialized data
			ContactData contact_data;

			// Deserialize each field of the contact data
			deserialize_name(contact_data.names, card_node.second.get_child("Name", boost::property_tree::ptree()));
			deserialize_phonetic(contact_data.phonetic_name, card_node.second.get_child("PhoneticName", boost::property_tree::ptree()));
			deserialize_nick_name(contact_data.nickname, card_node.second.get_child("NickNameRecord", boost::property_tree::ptree()));
			deserialize_work_info(contact_data.workinfo, card_node.second.get_child("WorkInfo", boost::property_tree::ptree()));
			deserialize_note(contact_data.note, card_node.second.get_child("Note", boost::property_tree::ptree()));

			// Deserialize telephones if the node exists
			auto telephone_child = card_node.second.get_child_optional("telephones");
			if (telephone_child) {
				for (const auto& telephone_node : *telephone_child) {
					TelephoneRecord telephone;
					deserialize_telephone(telephone, telephone_node.second);
					contact_data.telephones.push_back(telephone);
				}
			}

			// Deserialize emails if the node exists
			auto email_child = card_node.second.get_child_optional("emails");
			if (email_child) {
				for (const auto& email_node : *email_child) {
					EmailRecord email;
					deserialize_email(email, email_node.second);
					contact_data.emails.push_back(email);
				}
			}

			// Deserialize addresses if the node exists
			auto address_child = card_node.second.get_child_optional("addresses");
			if (address_child) {
				for (const auto& address_node : *address_child) {
					AddressRecord address;
					deserialize_address(address, address_node.second);
					contact_data.addresses.push_back(address);
				}
			}

			// Deserialize urls if the node exists
			auto url_child = card_node.second.get_child_optional("urls");
			if (url_child) {
				for (const auto& url_node : *url_child) {
					UrlRecord url;
					deserialize_url(url, url_node.second);
					contact_data.urls.push_back(url);
				}
			}

			// Deserialize events if the node exists
			auto event_child = card_node.second.get_child_optional("events");
			if (event_child) {
				for (const auto& event_node : *event_child) {
					EventRecord event;
					deserialize_event(event, event_node.second);
					contact_data.events.push_back(event);
				}
			}

			// Deserialize socials if the node exists
			auto social_child = card_node.second.get_child_optional("socials");
			if (social_child) {
				for (const auto& social_node : *social_child) {
					SocialNetRecord social;
					deserialize_social_net(social, social_node.second);
					contact_data.socials.push_back(social);
				}
			}

			// Deserialize relations if the node exists
			auto relation_child = card_node.second.get_child_optional("relations");
			if (relation_child) {
				for (const auto& relation_node : *relation_child) {
					RelationRecord relation;
					deserialize_relation(relation, relation_node.second);
					contact_data.relations.push_back(relation);
				}
			}

			// Add the deserialized contact data to the vector
			cards.push_back(contact_data);
		}
	}
	catch (const boost::property_tree::json_parser_error& e) {
		std::cerr << "JSON parser error: " << e.what() << std::endl;
		std::cerr << "Line: " << e.line() << std::endl;
		std::cerr << "Filename: " << e.filename() << std::endl;
	}
	catch (const boost::property_tree::xml_parser_error& e) {
		std::cerr << "XML parser error: " << e.what() << std::endl;
		std::cerr << "Line: " << e.line() << std::endl;
		std::cerr << "Filename: " << e.filename() << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "An error occurred: " << e.what() << std::endl;
	}
}

#endif

void BoostSerializer::deserialize_names(const boost::property_tree::ptree& pt, NameRecord& names) {
	if (pt.get<bool>("encoded", false)) {
		names.set_encoded_state();
	}
	names.family = pt.get<std::string>("family");
	names.personal = pt.get<std::string>("personal");
	names.father = pt.get<std::string>("father");
	names.address_form = pt.get<std::string>("address_form");
	names.suffix = pt.get<std::string>("suffix");
}

void BoostSerializer::deserialize_telephone(const boost::property_tree::ptree& pt, TelephoneRecord& telephone) {

	//telephone.is_encoded = pt.get<bool>("encoded");
	if (pt.get<bool>("encoded", false)) {
		telephone.set_encoded_state();
	}

	telephone.type = pt.get<std::string>("type");
	telephone.number = pt.get<std::string>("number");
	telephone.is_custom = pt.get<bool>("is_custom");
}

void BoostSerializer::deserialize_vcards(const std::string& filename, std::vector<ContactData>& cards) {
	// Determine file extension
	std::string file_extension = filename.substr(filename.find_last_of(".") + 1);

	// Create an empty property tree
	boost::property_tree::ptree pt;

	// Load the file into the property tree based on the file extension
	if (file_extension == "json") {
		boost::property_tree::read_json(filename, pt);
	}
	else if (file_extension == "xml") {
		boost::property_tree::read_xml(filename, pt);
	}
	else {
		// Unsupported file type, handle appropriately
		throw std::invalid_argument("Unsupported file type");
	}

	for (const auto& card_node : pt.get_child("cards")) {
		ContactData card;

		// Deserialize names
		if (auto names_node = card_node.second.get_child_optional("Name")) {
			deserialize_names(names_node.get(), card.names);
		}

		// Deserialize telephones
		if (auto telephones_node = card_node.second.get_child_optional("telephones")) {
			for (const auto& telephone_node : telephones_node.get()) {
				TelephoneRecord telephone;
				deserialize_telephone(telephone_node.second, telephone);
				card.telephones.push_back(telephone);
			}
		}

		cards.push_back(card);
	}
}













int main() {
	system("chcp 65001 >NUL");
	std::vector<ContactData> cards;

	const std::string input_file{ "ZZZ.vcf" };
	const std::string output_file{ "TESTITTO.vcf" };
	const std::string output_json{ "output.json" };
	const std::string output_xml{ "output.xml" };

	//VcfParser vcf_parser;
	//vcf_parser.parse_vcf(input_file, cards);

	//JsonMakerBoost jmb;
	//jmb.deserialize_vcards(output_json, cards);

	JsonMakerBoost jmb;
	//jmb.serialize_vcards(cards);
	//jmb.print_to_console();
	//jmb.print_to_file(output_json);
	//jmb.deserialize_vcards(output_json, cards);

	XmlMakerBoost xmb;
	//xmb.serialize_vcards(cards);
	//xmb.print_to_console();
	//xmb.print_to_file(output_xml);
	//xmb.deserialize_vcards(output_xml, cards);

	//VcfPrinter vcf_printer;
	//vcf_printer.print_to_console(cards);
	//vcf_printer.print_to_file(cards, output_file);


	return 0;
}

#endif // NOUSE