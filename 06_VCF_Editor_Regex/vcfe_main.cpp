#define NOUSE
#ifdef NOUSE

#include <iostream>
#include <string>

#include "vcfe_data_containers.hpp"
#include "vcfe_vcf_parser.hpp"
#include "vcfe_vcf_printer.hpp"
#include "vcfe_serializer.hpp"

/*
*/
////////////////
// 

#ifdef OOOOOO

#endif


int main() {
	system("chcp 65001 >NUL");
	std::vector<ContactData> cards;

	const std::string input_file { "AAA.vcf" };
	const std::string output_file{ "TESTITTO.vcf" };
	const std::string output_json{ "output.json" };
	const std::string output_xml { "output.xml" };

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
	xmb.deserialize_vcards(output_xml, cards);

	//VcfPrinter vcf_printer;
	//vcf_printer.print_to_console(cards);
	//vcf_printer.print_to_file(cards, output_file);a

	return 0;
}

#endif // NOUSE