
#include <vector>
#include <memory>
#include "vcfe_ptree_parser.hpp"
#include "vcfe_vcf_parser.hpp"
#include "vcfe_vcf_printer.hpp"

enum class FileExtension {
	VCF, JSON, XML
};


class ContactSerializer {
	VcfParser vcf_parser;
	VcfPrinter vcf_printer;
	std::vector<ContactData> cards;
	mutable PropertyTreeSerializer ptree_handler;
public:
	std::vector<ContactData>& get_all_contacts() {
		return cards;
	}
	void parse_file(const std::string& filename) {

		std::string file_extension = filename.substr(filename.find_last_of(".") + 1);
		ptree_handler.unset_new_file_handled();

		if (file_extension == "vcf" || file_extension == "VCF") {
			vcf_parser.parse_vcf(filename, cards);
			return;
		}
		else {
			ptree_handler.deserialize_vcards(filename, cards);
			return;
		}
	}
public:
	void print_to_file(const std::string& filename, const FileExtension& fe) const {
		switch (fe){
		case FileExtension::VCF:
			vcf_printer.print_to_file(cards, filename);
			return;
		case FileExtension::JSON:
			ptree_handler.serialize_vcards(cards);
			ptree_handler.print_json_to_file(filename);
			return;
		case FileExtension::XML:
			ptree_handler.serialize_vcards(cards);
			ptree_handler.print_xml_to_file(filename);
			return;
		default:
			std::cerr << "\n\n Invalid file extention!!! \n\n";
			return;
		}
	}
public:
	void print_to_console(const FileExtension& fe) const {
		switch (fe) {
		case FileExtension::VCF:
			vcf_printer.print_to_console(cards);
			return;
		case FileExtension::JSON:
			ptree_handler.serialize_vcards(cards);
			ptree_handler.print_json_to_console();
			return;
		case FileExtension::XML:
			ptree_handler.serialize_vcards(cards);
			ptree_handler.print_xml_to_console();
			return;
		default:
			std::cerr << "\n\n Invalid extention!!! \n\n";
			return;
		}
	}
};