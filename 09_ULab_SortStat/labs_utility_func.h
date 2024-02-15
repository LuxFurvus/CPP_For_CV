#pragma once
#ifndef _LABS_UTILITY_FUNC_
#define _LABS_UTILITY_FUNC_

#include <iostream>
#include <vector>

struct Settings {
	int mode{ 0 };
	int iterations{ 1 };
	int step{ 5 };
	bool use_multithreading{ false };
};

std::ostream& operator<<(std::ostream& os, const std::vector<int>& arr);
Settings start_program();
std::vector<int64_t> get_tablerow_data(int arr_size, bool use_mt);

#endif // _LABS_UTILITY_FUNC_
