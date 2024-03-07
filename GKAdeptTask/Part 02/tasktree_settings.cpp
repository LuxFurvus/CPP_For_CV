
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include "tasktree_settings.hpp"

std::string Settings::get_settings(bool clear)
{
	std::string temp(settings_line);
	if (clear) settings_line.clear();
	return temp;
}

void Settings::parse_settings(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << filename << " cannot be opened!\n";
		return; // Exit if the file cannot be opened
	}

	settings_line.clear(); // Ensure the settings line is empty before starting
	std::string line;
	while (std::getline(file, line)) {
		std::string key, value;
		std::istringstream is_line(line);
		if (!std::getline(is_line, key, ':')) {
			continue; // Skip if the line doesn't contain ':'
		}
		if (!std::getline(is_line, value)) {
			continue; // Skip if there is no value after ':'
		}
		// Trim leading whitespaces from the value
		value.erase(value.begin(), std::find_if(
			value.begin(), value.end(),
			[](unsigned char ch) {return !std::isspace(ch); }
		));

		// Concatenate the key and value to settings_line
		if (!settings_line.empty()) {
			settings_line.append(" ");
		}
		settings_line.append(key).append("=").append(value);
	}

	file.close();
}
