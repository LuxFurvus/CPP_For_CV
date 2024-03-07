#pragma once
#ifndef TABLE_PRINTER_H
#define TABLE_PRINTER_H

#include <vector>
#include <string>
#include <fstream>

class TablePrinter {
private:
	std::streambuf* orig_buf;
	std::ofstream out_file;
	std::vector<std::string> topHeaders; // Top headers
	std::vector<std::string> headers; // Column headers
	int work_mode{ 0 };
	int colWidth{ 10 }; // Width of each column
	std::string log_file{ "lab01_log.txt" };
	void printLine(int cols, int width);
public:
	explicit TablePrinter(int mode);
	void printTable(const std::vector<std::vector<int64_t>>& data);
};

#endif // TABLE_PRINTER_H
