#define NOUSE
#ifdef NOUSE

#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "vcfe_data_containers.h"
#include "vcfe_vcf_parser.h"
#include "vcfe_vcf_printer.h"
#include "vcfe_json_maker.h"

/*
*/
////////////////
// 


int main() {
	system("chcp 65001 >NUL");
	std::vector<ContactData> cards;
	const std::string input_file{ "AAA.vcf" };
	const std::string output_file{ "TESTITTO.vcf" };

	VcfParser vcf_parser;
	vcf_parser.parse_vcf(input_file, cards);

	//VcfPrinter vcf_printer;
	//vcf_printer.print_to_console(cards);
	//vcf_printer.print_to_file(cards, output_file);

	JsonMakerBoost jmb;
	jmb.make_json(cards);
	std::cout << "Serialized using Boost Property Tree:\n";
	boost::property_tree::write_json(std::cout, jmb.get_ptree(), true);

	return 0;
}

#endif // NOUSE