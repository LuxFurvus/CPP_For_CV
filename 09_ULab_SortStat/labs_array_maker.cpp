#include <random>

#include "labs_array_maker.h"
#include "labs_utility_func.h"

std::vector<int> ArrayMaker::make_asc_array() {
	std::vector<int> arr;
	arr.reserve(arr_size + 1);

	int asc_end{ min_element + arr_size };

	for (int i = min_element; i <= asc_end; ++i) {
		arr.emplace_back(i);
	}
	return arr;
}
std::vector<int> ArrayMaker::make_desc_array() {
	std::vector<int> arr;
	arr.reserve(arr_size + 1);

	int desc_start{ min_element + arr_size };

	for (int i = desc_start; i >= min_element; --i) {
		arr.emplace_back(i);
	}
	return arr;
}
std::vector<int> ArrayMaker::make_rand_array() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(min_element, max_element);

	std::vector<int> arr;
	arr.reserve(arr_size + 1);

	for (size_t i = 0; i <= arr_size; ++i) {
		arr.emplace_back(distrib(gen));
	}
	return arr;
}