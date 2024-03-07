#include <vector>
#include <algorithm>
#include "labs_sort_runner.h"

void SortRunner::is_sorted_control(const std::vector<int>& arr, const char* fn) {
	if (!std::is_sorted(arr.begin(), arr.end())) {
		std::cerr << "SORTING FAILED IN " << fn << " !!!\n";
		exit(-1);
	}
}

// ������ ��������� ������� �� ��������, 
// ��� ��� �� ���� ������� ��� ����� ������ ���������� ����������,
// �� �� �� ���������
void SortRunner::dosort(std::vector<int> arr, SortBase* sofu){
	// ������� ��������� �����������
	std::cout << "_________________________________\n";
	std::cout << "____________" << sofu->get_sort_type() << "______________\n";

	// ������� ����������� �����
	sd.check_sum = 0;
	for (const int& i : arr) {
		sd.check_sum += i;
	}

	// ������� �������� ������
	std::cout << "Initial array: " << arr << "\n\n";

	// ��������� ����������
	(*sofu)(arr, sd);

	// ���������, ������ �� ���������� �������
	is_sorted_control(arr, __func__);

	// ����� ���������� ����������
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
	// ������ ��� �������� ������������ �����
	std::vector<std::vector<int>> series;

	// ���������, �� ���� �� �������� ������
	if (!arr.empty()) {
		// �������� � ������� ��������
		series.emplace_back(); // ��������� ����� ������ ��� �����
		series.back().push_back(arr[0]); // ��������� ������ ������� � ������ �����

		// ���������� ��������, ������� �� �������
		int arr_size { static_cast<int>(arr.size()) };
		for (int i = 1; i < arr_size; ++i) {
			// ���� ������� ������� ������ �����������, ��������� ��� � ������� �����
			if (arr[i] > arr[i - 1]) {
				series.back().push_back(arr[i]);
			}
			// ����� �������� ����� �����
			else {
				series.emplace_back();
				series.back().push_back(arr[i]);
			}
		}
	}

	// �������� �����
	std::cout << "Series (" << series.size() << "): ";
	for (const auto& s : series) {
		for (int num : s) {
			std::cout << num << " ";
		}
		std::cout << "|";
	}
	// ��������� ����� ��������� ������
	std::cout << "\n";
}