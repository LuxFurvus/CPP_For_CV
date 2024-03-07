#include <iostream>
#include <iomanip>
#include <fstream>
#include "labs_utility_func.h"
#include "labs_table_printer.h"

TablePrinter::TablePrinter(int mode) : colWidth(10), work_mode(mode) {
	topHeaders = { "SELECTION Sort", "SHELL Sort", "HEAP Sort" };
	headers = { "DESC   ", "ACS   ", "RAND   ", "DESC   ", "ACS   ", "RAND   ", "DESC   ", "ACS   ", "RAND   " };

	if (work_mode == 1) {
		std::cout << "Table construction started. Please, wait...\n\n";

		// Save the original buffer of std::cout
		orig_buf = std::cout.rdbuf();

		// Open a file for output
		out_file.open(log_file);
		if (!out_file) {
			std::cerr << "Error opening file\n";
			//return;
		}

		// Redirect std::cout to file
		std::cout.rdbuf(out_file.rdbuf());
	}
}

void TablePrinter::printLine(int cols, int width) {
	std::cout << "+";  // Leftmost column line start
	for (int j = 0; j < colWidth; ++j) {  // Width for the leftmost column
		std::cout << "-";
	}

	for (int i = 0; i < cols; ++i) {
		std::cout << "+";
		for (int j = 0; j < width; ++j) {
			std::cout << "-";
		}
	}
	std::cout << "+" << std::endl;
}

void TablePrinter::printTable(const std::vector<std::vector<int64_t>>& data) {
	if (work_mode == 1) {
		// Restore the original buffer
		std::cout.rdbuf(orig_buf);

		// Close the file
		out_file.close();

		std::cout << "For program logs, see " << log_file << "\n\n";
	}
	const int rows = static_cast<int>(data.size());
	const int cols = static_cast<int>(rows > 0 ? data[0].size() : 0);

	// Print top headers (including an empty space for the leftmost column)
	printLine(cols, colWidth);
	std::cout << "|" << std::setw(colWidth - 1);  // Leftmost header space
	for (const auto& topHeader : topHeaders) {
		std::cout << "" << std::setw(3 * colWidth - 1) << topHeader;
	}
	std::cout << "|\n";

	// Print the headers (including the leftmost header)
	printLine(cols, colWidth);
	std::cout << "|" << std::setw(colWidth) << "Elem.Num ";  // Leftmost header
	for (const auto& header : headers) {
		std::cout << "|" << std::setw(colWidth) << header;
	}
	std::cout << "|\n";

	// Print the table data
	for (int i = 0; i < rows; ++i) {
		printLine(cols, colWidth);
		std::cout << "|" << std::setw(colWidth) << data[i][0];
		//for (const auto& elem : data[i]) {
		for (int z = 1; z < data[i].size(); ++z) {
			std::cout << "|" << std::setw(colWidth) << data[i][z];
		}
		std::cout << "|\n";
	}
	printLine(cols, colWidth);
}