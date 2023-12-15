#include <fstream>
#include <string>
#include <vector>

void collect_lines(std::vector<std::string>& lines) {
	std::ifstream file("AAA.vcf", std::ios::in);
	std::vector<std::string> newlines;

	if (!file.is_open()) {
		printf("Can't open the file!\n");
		return;
	}

	std::string oneline;

	while (std::getline(file, oneline)) {
		if (oneline.empty()) {
			continue;
		}

		if (oneline[0] == '=' || oneline[0] == ';') {
			//oneline.erase(oneline.begin());
			auto& current_string = lines[lines.size() - 1];
			current_string.erase(current_string.end() - 1);
			current_string.append(oneline);
			continue;
		}

		lines.push_back(oneline);
	}
}