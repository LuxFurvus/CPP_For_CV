#define NOUSE
#ifdef NOUSE

#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include <cstdarg>

#include "vcfe_serializer.h"
/*
*/
////////////////
// запрос на создание таблиц контактов
// функция на перевод данных из таблиц в программу
// функция на перевод данных из программы в таблицы


class PGDBHandler {
public:

};

#ifdef OOOOOO

#endif

int main() {
	system("chcp 65001 >NUL");
	std::vector<ContactData> cards;

	const std::string input_file { "AAA.vcf" };
	const std::string output_file{ "OUTPUT_CARDS" };

	ContactSerializer cas;
	cas.parse_file(input_file);
	cas.print_to_file(output_file, FileExtension::JSON);
	cas.print_to_file(output_file, FileExtension::XML);
	cas.print_to_file(output_file, FileExtension::VCF);

	return 0;
}

#endif // NOUSE