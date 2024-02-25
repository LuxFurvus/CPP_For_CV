#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <future>
#include <thread>
#include <functional>

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
        if (line.empty()) continue;
        // Parse lines based on keywords
        parse_setting_line(line);
    }
}

int SiteParser::make_https_request(const std::string& url, int num) {
    
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(("curl -s " + url).c_str(), "r"), pclose);
    
    if (!pipe) {
        std::cerr << "Error opening pipe" << std::endl;
        return -1;
    }

    const size_t bufferSize = 4096;
    std::vector<char> buffer(bufferSize, '\0');

    std::string resp;

    // Read the response from the curl command
    while (fgets(buffer.data(), bufferSize, pipe.get()) != NULL) {
        resp += buffer.data();
    }

    if (resp.find(search_line) != std::string::npos) {
        return num;
    }

    return -1;
}

std::string SiteParser::make_real_url(std::string input, int number) {
	std::string number_str = std::to_string(number);
	size_t pos = input.find("***");
	if (pos != std::string::npos) {
		input.replace(pos, 3, number_str);
	}
	return input;
}

void SiteParser::parse_site() {

    correct_urls.reserve(end_num - start_num);

    // Limit the number of concurrent requests to a reasonable number
    // std::min to avoid over-allocation in case of a small range [start_num, end_num).
    uint32_t max_concurrent_requests {std::min(
        static_cast<uint32_t>(end_num - start_num),
        std::max(1u, std::thread::hardware_concurrency() * 2))};

    std::vector<std::future<int>> futures;
    futures.reserve(max_concurrent_requests);

    // Local lambda to DRY in for loops below
    auto collect_futures = [&](std::future<int>& fut)-> bool {
        if(fut.valid() == false) return true;
        int result = fut.get();
        if (result == -1) return true;
        correct_urls.emplace_back(make_real_url(url_template, result));
        return false;
    };

    for (int i = start_num; i < end_num; ++i) {
        std::string url_todo {make_real_url(url_template, i)};

        // Launch asynchronous tasks
        futures.push_back(std::async(std::launch::async, 
            &SiteParser::make_https_request, this, url_todo, i));

        // If we've reached the maximum number of concurrent tasks, wait for them to complete
        if (futures.size() >= max_concurrent_requests) {
            for (auto &fut : futures) {
                if(collect_futures(fut)) continue;
            }
            // Clear completed futures
            futures.clear();            
        }

        // Output progress
        std::cout 
        << "Parsing: [ " << url_todo 
        << " ] Found " << get_correct_urls_num() 
        << " correct urls" << "\r" << std::flush;
    }

    // Wait for any remaining asynchronous tasks to complete
    for (auto &fut : futures) {
        if(collect_futures(fut)) continue;
    }

    std::cout << "\nParsing complete! For parsing results, see " << output_file << "\n";
}

void SiteParser::print_parse_result(){
	std::ofstream outFile;
	outFile.open(output_file);

	if (!outFile.is_open()) {
		std::cerr << "Error opening the input file.\n";
		return;
	}

    int counter{0};

	for(const std::string& url : correct_urls){
        outFile << url;
        ++counter;
        (counter % 10 == 0)? (outFile << "\n\n") : (outFile <<"\n");
	}
    
	outFile.close();
}
