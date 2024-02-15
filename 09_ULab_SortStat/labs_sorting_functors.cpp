#include <iostream>
#include <vector>

#include "labs_sort_runner.h"
#include "labs_sorting_functors.h"

// ���������� ������ �������
void SelectionSortFunctor::operator()(std::vector<int>& arr, SortData& sd) {
	int n = static_cast<int>(arr.size());

	for (int i = 0; i < n - 1; i++) {
		// ������� ������ ������������ �������� � ����������������� ����� �������
		int min_index = i;
		for (int j = i + 1; j < n; j++) {
			sd.comparisons++;  // ��������� ���������
			if (arr[j] < arr[min_index]) {
				min_index = j;
			}
		}

		// ���������, ����� �� �����
		if (min_index != i) {
			std::swap(arr[i], arr[min_index]);
			++sd.swaps;  // ��������� �����
		}
		++sd.steps;
		std::cout << "\tStep (" << sd.steps << "), ";
		print_series(arr);

		//��������� ����������� �����
		check_sum_control(arr, sd.check_sum, __func__);
	}
}

// ���������� ������� �����
void ShellSortFunctor::operator()(std::vector<int>& arr, SortData& sd) {
	int n = static_cast<int>(arr.size());

	std::vector<int> gaps; // ������ ��� �������� ������������������ �����
	int gap = 1;

	// ��������� ������������������ ����� �� ������� �����
	while (gap < n) {
		gaps.push_back(gap);
		gap = 2 * gap + 1;
	}

	// ����� ������������������ �����
	std::cout << "Knuth Steps Sequence: ";
	std::cout << gaps << "\n";

	// �������� ���� ����������
	for (int i = static_cast<int>(gaps.size() - 1); i >= 0; --i) {
		gap = gaps[i]; // ��������� �������� �������� ����

		// ������ �� ���������, ������� � �������� ����
		for (int j = gap; j < n; j++) {
			int temp = arr[j]; // ������� ������� ��� ���������
			int k = j;
			// ����� ������������ ���
			std::cout << "Compare: " << arr[k - gap] << " and " << temp;

			// ����� ���������, ������� ������ �������� �������� temp
			for (k = j; k >= gap && (arr[k - gap] > temp); k -= gap) {
				arr[k] = arr[k - gap]; // ����� ��������
				++sd.comparisons;  // ���������� �������� ���������
			}
			arr[k] = temp; // ������� �������� � ���������� �����

			// ���� ��������� ������������
			if (j != k) ++sd.swaps;

			// ��������� ����������� �����
			check_sum_control(arr, sd.check_sum, __func__);
			++sd.steps;

			std::cout << ", ";
			print_series(arr);
		}
	}
}

// ������� ��� "�����������" �������� ���� �� ��������
void HeapSortFunctor::heapify(std::vector<int>& arr, int n, int i, bool& changed, int64_t& comparisons, int64_t& swaps) {
	int largest = i; // �������������� ���������� ������� ��� ������
	int l = 2 * i + 1; // ����� = 2*i + 1
	int r = 2 * i + 2; // ������ = 2*i + 2

	// ������� ���������� � ������� ����� � ��� �������� ���������
	std::cout << "\tRoot: " << arr[i] << "[" << i << "]";
	if (l < n) std::cout << ", Left: " << arr[l] << "[" << l << "]";
	if (r < n) std::cout << ", Right: " << arr[r] << "[" << r << "]";
	std::cout << "\n";

	// ���� ����� �������� ������� ������ �����
	if (l < n) {
		++comparisons; // ����������� ������� ���������
		if (arr[l] > arr[largest]) {
			largest = l;
		}
	}

	// ���� ������ �������� ������� ������, ��� ����� ������� �� ������ ������
	if (r < n) {
		++comparisons; // ����������� ������� ���������
		if (arr[r] > arr[largest]) {
			largest = r;
		}
	}

	// ���� ����� ������� ������� �� ������
	if (largest != i) {
		std::swap(arr[i], arr[largest]);
		++swaps; // ����������� ������� �������
		changed = true;

		// ���������� ���������� ������� � ���������
		heapify(arr, n, largest, changed, comparisons, swaps);
	}
}

// ������������� ����������
void HeapSortFunctor::operator()(std::vector<int>& arr, SortData& sd) {
	int n = static_cast<int>(arr.size());

	// ���������� �������� (�������������� ������)
	for (int i = n / 2 - 1; i >= 0; i--) {
		bool changed = false;
		heapify(arr, n, i, changed, sd.comparisons, sd.swaps);
		if (changed) {
			print_series(arr);
			std::cout << "\n\n";

			// ����������� ������� ��������
			++sd.steps;

			//��������� ����������� �����
			check_sum_control(arr, sd.check_sum, __func__);
		}
	}

	// ���� �� ������ ��������� �������� �� ��������
	for (int i = n - 1; i >= 0; i--) {
		std::swap(arr[0], arr[i]);

		// ����������� ������� �������
		++sd.swaps;

		bool changed = false;
		heapify(arr, i, 0, changed, sd.comparisons, sd.swaps);

		if (changed) {
			print_series(arr);
			std::cout << "\n";

			// ����������� ������� ��������
			++sd.steps;

			//��������� ����������� �����
			check_sum_control(arr, sd.check_sum, __func__);
		}
	}
}