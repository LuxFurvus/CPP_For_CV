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

//enum class PrintDestination {
//	CONSOLE, FILE
//};



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

	//JsonMakerBoost jmb;
	//jmb.make_json(cards);
	//jmb.print_json_to_console();
	//jmb.print_json_to_file("output.json");

	XmlMakerBoost xmb;
	xmb.make_xml(cards);
	xmb.print_xml_to_console();
	xmb.print_xml_to_file("output.xml");


	return 0;
}

#endif // NOUSE