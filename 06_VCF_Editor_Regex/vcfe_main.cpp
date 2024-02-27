#define NOUSE
#ifdef NOUSE

#include <string>
#include <vector>
#include <locale>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
extern "C" {
#include "vcfe_encode_to_hex.h"
}
#include "vcfe_data_containers.h"
#include "vcfe_utility_functions.h"
#include "vcfe_printer_functions.h"
////////////////

/*



*/

////////////////

////////////////

////////////////

int main() {
	system("chcp 65001 >NUL");
	//setlocale(LC_ALL, "en_US.utf8");

	std::vector<std::string> lines;

	collect_lines(lines, "AAA.vcf");

	std::vector<ContactData> cards;

	analyse_lines(lines, cards);

	show_cards(cards);

	//print_vcf(cards, "D:\\ForCPP\\CPP_For_CV\\06_VCF_Editor_Regex\\TESTITTO.vcf");

	return 0;
}

#endif // NOUSE

//
//
//Создать профиль с текстом на латинице
//Может ли карточка содержать несколько NOTE
//Увеличить количество дат
//
//