#pragma once
#ifndef _TASKTREE_SETTINGS_HPP_
#define _TASKTREE_SETTINGS_HPP_

#include <string>

class Settings
{
	std::string settings_line;

public:
	Settings() = default;
	void parse_settings(const std::string& filename);
	std::string get_settings(bool clear = false);
};

#endif // _TASKTREE_SETTINGS_HPP_
