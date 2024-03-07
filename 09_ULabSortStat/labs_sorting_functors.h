#pragma once

#ifndef SORTING_FUNCTORS_H
#define SORTING_FUNCTORS_H

#include <vector>
#include "labs_sort_runner.h"

// Declaration of the SelectionSortFunctor class
class SelectionSortFunctor : public SortBase {
public:
	void operator()(std::vector<int>& arr, SortData& sd) override;
	inline std::string get_sort_type() override { return "SELECTION SORT"; }
};

// Declaration of the ShellSortFunctor class
class ShellSortFunctor : public SortBase {
public:
	void operator()(std::vector<int>& arr, SortData& sd) override;
	inline std::string get_sort_type() override { return "SHELL SORT"; }
};

// Пирамидальная сортировка
class HeapSortFunctor : public SortBase {
private:
	// Функция для "просеивания" элемента вниз по пирамиде
	void heapify(std::vector<int>& arr, int n, int i, bool& changed, int64_t& comparisons, int64_t& swaps);
public:
	void operator()(std::vector<int>& arr, SortData& sd) override;
	inline std::string get_sort_type() override { return "HEAP SORT"; }
};

#endif // SORTING_FUNCTORS_H
