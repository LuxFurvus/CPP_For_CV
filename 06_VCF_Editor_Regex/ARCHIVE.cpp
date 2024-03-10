
#ifdef TEMP_ARCHIVE

void serialization_test() {
	std::vector<ContactData> cards;
	const std::string input_file{ "AAA.vcf" };
	const std::string output_file{ "OUTPUT_CARDS" };

	ContactSerializer cas;
	cas.parse_file(input_file);
	cas.print_to_file(output_file, FileExtension::JSON);
	cas.print_to_file(output_file, FileExtension::XML);
	cas.print_to_file(output_file, FileExtension::VCF);
}

#endif