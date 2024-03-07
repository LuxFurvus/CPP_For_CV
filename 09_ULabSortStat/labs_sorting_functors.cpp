#include <iostream>
#include <vector>

#include "labs_sort_runner.h"
#include "labs_sorting_functors.h"

// Сортировка прямым выбором
void SelectionSortFunctor::operator()(std::vector<int>& arr, SortData& sd) {
	int n = static_cast<int>(arr.size());

	for (int i = 0; i < n - 1; i++) {
		// Находим индекс минимального элемента в неотсортированной части массива
		int min_index = i;
		for (int j = i + 1; j < n; j++) {
			sd.comparisons++;  // учитываем сравнение
			if (arr[j] < arr[min_index]) {
				min_index = j;
			}
		}

		// Проверяем, нужен ли обмен
		if (min_index != i) {
			std::swap(arr[i], arr[min_index]);
			++sd.swaps;  // учитываем обмен
		}
		++sd.steps;
		std::cout << "\tStep (" << sd.steps << "), ";
		print_series(arr);

		//Проверяем контрольную сумму
		check_sum_control(arr, sd.check_sum, __func__);
	}
}

// Сортировка методом Шелла
void ShellSortFunctor::operator()(std::vector<int>& arr, SortData& sd) {
	int n = static_cast<int>(arr.size());

	std::vector<int> gaps; // Вектор для хранения последовательности шагов
	int gap = 1;

	// Генерация последовательности шагов по формуле Кнута
	while (gap < n) {
		gaps.push_back(gap);
		gap = 2 * gap + 1;
	}

	// Вывод последовательности шагов
	std::cout << "Knuth Steps Sequence: ";
	std::cout << gaps << "\n";

	// Основной цикл сортировки
	for (int i = static_cast<int>(gaps.size() - 1); i >= 0; --i) {
		gap = gaps[i]; // Установка текущего значения шага

		// Проход по элементам, начиная с текущего шага
		for (int j = gap; j < n; j++) {
			int temp = arr[j]; // Текущий элемент для сравнения
			int k = j;
			// Вывод сравниваемых пар
			std::cout << "Compare: " << arr[k - gap] << " and " << temp;

			// Сдвиг элементов, которые больше текущего элемента temp
			for (k = j; k >= gap && (arr[k - gap] > temp); k -= gap) {
				arr[k] = arr[k - gap]; // Сдвиг элемента
				++sd.comparisons;  // Увеличение счетчика сравнений
			}
			arr[k] = temp; // Вставка элемента в правильное место

			// Если произошла перестановка
			if (j != k) ++sd.swaps;

			// Проверяем контрольную сумму
			check_sum_control(arr, sd.check_sum, __func__);
			++sd.steps;

			std::cout << ", ";
			print_series(arr);
		}
	}
}

// Функция для "просеивания" элемента вниз по пирамиде
void HeapSortFunctor::heapify(std::vector<int>& arr, int n, int i, bool& changed, int64_t& comparisons, int64_t& swaps) {
	int largest = i; // Инициализируем наибольший элемент как корень
	int l = 2 * i + 1; // левый = 2*i + 1
	int r = 2 * i + 2; // правый = 2*i + 2

	// Выводим информацию о текущем корне и его дочерних элементах
	std::cout << "\tRoot: " << arr[i] << "[" << i << "]";
	if (l < n) std::cout << ", Left: " << arr[l] << "[" << l << "]";
	if (r < n) std::cout << ", Right: " << arr[r] << "[" << r << "]";
	std::cout << "\n";

	// Если левый дочерний элемент больше корня
	if (l < n) {
		++comparisons; // Увеличиваем счетчик сравнений
		if (arr[l] > arr[largest]) {
			largest = l;
		}
	}

	// Если правый дочерний элемент больше, чем самый большой на данный момент
	if (r < n) {
		++comparisons; // Увеличиваем счетчик сравнений
		if (arr[r] > arr[largest]) {
			largest = r;
		}
	}

	// Если самый большой элемент не корень
	if (largest != i) {
		std::swap(arr[i], arr[largest]);
		++swaps; // Увеличиваем счетчик обменов
		changed = true;

		// Рекурсивно просеиваем элемент в поддереве
		heapify(arr, n, largest, changed, comparisons, swaps);
	}
}

// Пирамидальная сортировка
void HeapSortFunctor::operator()(std::vector<int>& arr, SortData& sd) {
	int n = static_cast<int>(arr.size());

	// Построение пирамиды (перегруппируем массив)
	for (int i = n / 2 - 1; i >= 0; i--) {
		bool changed = false;
		heapify(arr, n, i, changed, sd.comparisons, sd.swaps);
		if (changed) {
			print_series(arr);
			std::cout << "\n\n";

			// Увеличиваем счетчик проходов
			++sd.steps;

			//Проверяем контрольную сумму
			check_sum_control(arr, sd.check_sum, __func__);
		}
	}

	// Один за другим извлекаем элементы из пирамиды
	for (int i = n - 1; i >= 0; i--) {
		std::swap(arr[0], arr[i]);

		// Увеличиваем счетчик обменов
		++sd.swaps;

		bool changed = false;
		heapify(arr, i, 0, changed, sd.comparisons, sd.swaps);

		if (changed) {
			print_series(arr);
			std::cout << "\n";

			// Увеличиваем счетчик проходов
			++sd.steps;

			//Проверяем контрольную сумму
			check_sum_control(arr, sd.check_sum, __func__);
		}
	}
}