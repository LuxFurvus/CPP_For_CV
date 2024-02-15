#include <iostream>
#include <vector>
#include <clocale>

#include "labs_utility_func.h"
#include "labs_array_maker.h"
#include "labs_utility_func.h"
#include "labs_sort_runner.h"
#include "labs_sorting_functors.h"
#include "labs_table_printer.h"

int main() {
	do {
		// ��������� ������� �������� UTF-8 ��� ������ � ������� ��������
		system("chcp 65001 >NUL");
		// ��������� ������������� ������ ���  UTF-8
		setlocale(LC_ALL, "en_US.utf8");

		// �������� ��������� ���������
		Settings stt = start_program();

		// ������� ������ ��� ������ ������� � �����
		TablePrinter printer(stt.mode);

		std::vector<std::vector<int64_t>> data;
		data.reserve(stt.iterations);

		int arr_size_max{ stt.step * stt.iterations };
		for (int i = stt.step; i <= arr_size_max; i += stt.step) {
			// ��������� ������� ��������������� �������
			data.emplace_back(get_tablerow_data(i, stt.use_multithreading));
		}

		// ������� ������� � �������
		printer.printTable(data);

		// ��������� ������ ���������, ���� ������������ ����� �����
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cin.get();
	} while (true);

	return 0;
}