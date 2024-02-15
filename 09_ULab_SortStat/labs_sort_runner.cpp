#include <vector>
#include <algorithm>
#include "labs_sort_runner.h"

void SortRunner::is_sorted_control(const std::vector<int>& arr, const char* fn) {
	if (!std::is_sorted(arr.begin(), arr.end())) {
		std::cerr << "SORTING FAILED IN " << fn << " !!!\n";
		exit(-1);
	}
}

// Массив намеренно передан по значению, 
// так как от этой функции нам нужна только статистика сортировки,
// но не ее результат
void SortRunner::dosort(std::vector<int> arr, SortBase* sofu){
	// Выводим начальный разделитель
	std::cout << "_________________________________\n";
	std::cout << "____________" << sofu->get_sort_type() << "______________\n";

	// Считаем контрольную сумму
	sd.check_sum = 0;
	for (const int& i : arr) {
		sd.check_sum += i;
	}

	// Выводим исходный массив
	std::cout << "Initial array: " << arr << "\n\n";

	// Выполняем сортировку
	(*sofu)(arr, sd);

	// Проверяем, прошла ли сортировка успешно
	is_sorted_control(arr, __func__);

	// Вывод статистики сортировки
	std::cout
		<< "\nComparisons: " << sd.comparisons
		<< ", Swaps: " << sd.swaps
		<< ", Steps: " << sd.steps << "\n";
	std::cout << "================================\n\n\n";
}

void SortBase::check_sum_control(const std::vector<int>& arr, int64_t check_sum, const char* fn) {
	int local_check_sum{ 0 };
	for (const int& i : arr) {
		local_check_sum += i;
	}
	if (local_check_sum != check_sum) {
		std::cerr << "CHECKSUM MISMATCH IN " << fn << " !!!\n";
		exit(-1);
	}
}

void SortBase::print_series(const std::vector<int>& arr) {
	// Вектор для хранения возрастающих серий
	std::vector<std::vector<int>> series;

	// Проверяем, не пуст ли исходный массив
	if (!arr.empty()) {
		// Начинаем с первого элемента
		series.emplace_back(); // Добавляем новый вектор для серии
		series.back().push_back(arr[0]); // Добавляем первый элемент в первую серию

		// Перебираем элементы, начиная со второго
		int arr_size { static_cast<int>(arr.size()) };
		for (int i = 1; i < arr_size; ++i) {
			// Если текущий элемент больше предыдущего, добавляем его в текущую серию
			if (arr[i] > arr[i - 1]) {
				series.back().push_back(arr[i]);
			}
			// Иначе начинаем новую серию
			else {
				series.emplace_back();
				series.back().push_back(arr[i]);
			}
		}
	}

	// Печатаем серии
	std::cout << "Series (" << series.size() << "): ";
	for (const auto& s : series) {
		for (int num : s) {
			std::cout << num << " ";
		}
		std::cout << "|";
	}
	// Завершаем вывод переносом строки
	std::cout << "\n";
}