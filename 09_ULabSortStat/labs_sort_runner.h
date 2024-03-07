#pragma once

#ifndef SORT_RUNNER_H
#define SORT_RUNNER_H

#include <iostream>
#include <vector>

#include "labs_utility_func.h"

struct SortData {
	int64_t comparisons{ 0 };  // ������� ��������� (C)
	int64_t swaps{ 0 };        // ������� ������� (M)
	int64_t check_sum{ 0 };	   // ����������� ����� (���� ���������)
	int64_t steps{ 0 };		   // ������� ��������
};

class SortBase {
protected:
	void check_sum_control(const std::vector<int>& arr, int64_t check_sum, const char* fn);
	void print_series(const std::vector<int>& arr);
public:
	virtual void operator()(std::vector<int>& arr, SortData& sd) = 0;
	virtual std::string get_sort_type() = 0;
	virtual ~SortBase() = default;
};

class SortRunner {
private:
	SortData sd;
protected:
	void is_sorted_control(const std::vector<int>& arr, const char* fn);
public:
	inline SortRunner(){}

	// ��������� ���������� ����������� ������� �� ����������� ��������
	void dosort(std::vector<int> arr, SortBase* sofu);

	// ���������� ����� ���������� ��������� � � ���������� ��������� �
	inline int64_t get_cm() const noexcept { return sd.comparisons + sd.swaps; }
};

#endif // SORT_RUNNER_H
