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
		// Установка кодовой страницы UTF-8 для вывода в консоль кирилицы
		system("chcp 65001 >NUL");
		// Установка универсальной локали под  UTF-8
		setlocale(LC_ALL, "en_US.utf8");

		// Получаем настройки программы
		Settings stt = start_program();

		// Создаем объект для печати таблицы в конце
		TablePrinter printer(stt.mode);

		std::vector<std::vector<int64_t>> data;
		data.reserve(stt.iterations);

		int arr_size_max{ stt.step * stt.iterations };
		for (int i = stt.step; i <= arr_size_max; i += stt.step) {
			// Заполняем таблицу статистическими данными
			data.emplace_back(get_tablerow_data(i, stt.use_multithreading));
		}

		// Выводим таблицу в консоль
		printer.printTable(data);

		// Повторяем работу программы, если пользователь этого хочет
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cin.get();
	} while (true);

	return 0;
}