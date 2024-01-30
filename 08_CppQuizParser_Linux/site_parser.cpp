#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>

#include "site_parser.h"

void SiteParser::remove_leading_spaces(std::string &input){
    size_t startIndex = input.find_first_not_of(" \t\n\r\f\v");
    
    if (startIndex == std::string::npos) {
        // Entire string is whitespace
        return;
    }

    input = input.substr(startIndex);
}

void SiteParser::parse_setting_line(const std::string &line){
    std::istringstream line_stream(line);
    std::string key, value;
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);

    remove_leading_spaces(value);

    if (key == "#URL Template") {
        url_template = value;
    } else if (key == "#Parse Start Index") {
        std::istringstream(value) >> start_num;
        if(start_num < 0) start_num = 0;
    } else if (key == "#Parse Final Index") {
        std::istringstream(value) >> end_num;
		++end_num;
        if(end_num < 0) end_num = -end_num;
    } else if (key == "#Discard URLs with this text") {
        search_line = value;
    } else if (key == "#Parsing output path and file") {
        output_file = value;
    }
}

void SiteParser::collect_settings(const char* file_name){
    // Read from the settings file
    std::ifstream inputFile(file_name);
    if (!inputFile) {
        std::cerr << "Error opening settings file.\n";
        exit(-1);
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        // Skip empty lines
        if (line.empty())
            continue;

        // Parse lines based on keywords
        parse_setting_line(line);
    }
}

bool SiteParser::make_https_request(const char* url) {
    
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(("curl -s " + std::string(url)).c_str(), "r"), pclose);
    
    if (!pipe) {
        std::cerr << "Error opening pipe" << std::endl;
        return false;
    }

    const size_t bufferSize = 4096;
    std::vector<char> buffer(bufferSize, '\0');

    response.clear();

    // Read the response from the curl command
    while (fgets(buffer.data(), bufferSize, pipe.get()) != NULL) {
        response += buffer.data();
    }

    return true;
}

void SiteParser::parse_site() {
    int counter = 0;

    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(max_url_size);

    correct_urls.reserve(end_num - start_num);

    const char* url_template_str = url_template.c_str();

    for (int i = start_num; i < end_num; ++i) {

        snprintf(buffer.get(), max_url_size, url_template_str, i);
        std::cout << "Parsing: [ " << buffer.get() 
        << " ] Found " << get_correct_urls_num() << " correct urls"
        << "\r" << std::flush;

        make_https_request(buffer.get());

        if (response.find(search_line) == std::string::npos) {
            correct_urls.emplace_back(buffer.get());
            ++counter;
        }

        response.clear();
    }

    std::cout << "\r\n" << std::flush;
    std::cout << "Parsing complete! For parsing results, see " 
    << output_file << "\n";
    
    return;
}

void SiteParser::print_parse_result(){
	std::ofstream outFile;
	outFile.open(output_file);

	if (!outFile.is_open()) {
		std::cerr << "Error opening the input file.\n";
		return;
	}

    int counter{0};

	for(const std::string& rr : correct_urls){
        outFile << rr;
        ++counter;
        (counter % 10 == 0)? (outFile << "\n\n") : (outFile <<"\n");
	}
    
	outFile.close();
}
