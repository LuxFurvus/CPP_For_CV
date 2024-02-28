#define NOUSE
#ifdef NOUSE

//int main() {
//	return 0;
//}
////////////////

/*

*/

////////////////

////////////////

////////////////

#include <iostream>
#include <string>
//#include <vector>
//#include <locale>
//#include <fstream>
//#include <iomanip>
//#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "nlohmann.hpp"

#include "vcfe_data_containers.h"

#include "vcfe_utility_functions.h"
#include "vcfe_vcf_parser.h"
#include "vcfe_vcf_printer.h"

// Serialize using Boost Property Tree
boost::property_tree::ptree json_names_boost(const Names& names) {
	boost::property_tree::ptree pt;
	pt.put("encoded", names.is_encoded());
	pt.put("Names.family", names.family);
	pt.put("Names.personal", names.personal);
	pt.put("Names.father", names.father);
	pt.put("Names.address_form", names.address_form);
	pt.put("Names.suffix", names.suffix);
	return pt;
}

// Serialize using nlohmann/json
nlohmann::json json_names_nlohmann(const Names& names) {
	nlohmann::json j;
	j["encoded"] = names.is_encoded();
	j["Names"]["family"] = names.family;
	j["Names"]["personal"] = names.personal;
	j["Names"]["father"] = names.father;
	j["Names"]["address_form"] = names.address_form;
	j["Names"]["suffix"] = names.suffix;
	return j;
}

int main() {
	system("chcp 65001 >NUL");
	std::vector<ContactData> cards;
	const std::string input_file{ "AAA.vcf" };
	const std::string output_file{ "TESTITTO.vcf" };

	VcfParser vcf_parser;
	vcf_parser.parse_vcf(input_file, cards);

	VcfPrinter vcf_printer;
	vcf_printer.print_to_console(cards);
	vcf_printer.print_to_file(cards, output_file);

// Create an instance of Names
	Names names;
	names.family = "Doe";
	names.personal = "John";
	names.father = "Jack";
	names.address_form = "123 Main St";
	names.suffix = "Jr.";
	names.set_encoded_state(); // Set encoded to true

	// Serialize using Boost Property Tree
	boost::property_tree::ptree pt = json_names_boost(names);

	// Serialize using nlohmann/json
	nlohmann::json j = json_names_nlohmann(names);

	// Output the serialized data
	std::cout << "Serialized using Boost Property Tree:\n";
	boost::property_tree::write_json(std::cout, pt, true);
	std::cout << "\n\nSerialized using nlohmann/json:\n";
	std::cout << j.dump(4) << '\n';

	return 0;
}

#endif // NOUSE