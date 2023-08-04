

// converts any input string (which starts from number) into an integer. If an input string starts with a letter or symbol, returns zero
int SafeIntInput(const std::string& str) {
	return std::atoi(str.c_str());
}


// counts number of lines in a file with accounts data
unsigned int LinesInFile(const char* FileName) {
	unsigned int numLines = 0;
	std::ifstream in(FileName);
	while (in.good()) {
		std::string line;
		std::getline(in, line);
		++numLines;
	}
	return numLines;
}

