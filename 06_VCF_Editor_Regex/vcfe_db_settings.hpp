#pragma once
#ifndef _VCFE_DB_SETTINGS_HPP_
#define _VCFE_DB_SETTINGS_HPP_

#include <string>

class DB_ConnectSettings
{
	std::string settings_line;

public:
	DB_ConnectSettings() = default;
	void parse_settings(const std::string& filename);
	std::string get_settings(bool clear = false);
};

#endif // _VCFE_DB_SETTINGS_HPP_
