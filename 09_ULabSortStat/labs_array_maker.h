#pragma once

#ifndef _ARRAY_MAKER_
#define _ARRAY_MAKER_

#include <vector>

class ArrayMaker {
private:
	const int min_element{ 100 };
	const int max_element{ 999 };
	const int arr_size{ 0 };
	std::vector<int> make_asc_array();
	std::vector<int> make_desc_array();
	std::vector<int> make_rand_array();
public:
	std::vector<int> asc_array;
	std::vector<int> desc_array;
	std::vector<int> rand_array;
	explicit ArrayMaker(int size) : arr_size(size) {
		asc_array = make_asc_array();
		desc_array = make_desc_array();
		rand_array = make_rand_array();
	}
};

#endif // _ARRAY_MAKER_
