
#pragma once
#ifndef _SITE_PARSER_H_
#define _SITE_PARSER_H_

#include <string>
#include <vector>
#include <memory>

class SiteParser{
private:
    std::vector<std::string> correct_urls;
    std::string response;
    std::string url_template;
    std::string search_line;
    std::string output_file;
    int start_num{0};
    int end_num{0};
    const int max_url_size{512};

    void remove_leading_spaces(std::string& input);
    void parse_setting_line(const std::string& line);
    void collect_settings(const char* file_name);
    std::string make_real_url(std::string input, int number);
    int make_https_request(const std::string& url, int num);

    inline const std::string& get_response() const noexcept { 
        return response; 
        }
    inline int get_correct_urls_num() const noexcept { 
        return correct_urls.size(); 
        }
    inline void set_url_template(const std::string& url) noexcept { 
        url_template = url; 
        }
    inline void set_search_line(const std::string& line) noexcept { 
        search_line = line; 
        }
    inline void set_parse_range(int start, int end) noexcept { 
        start_num = start; 
        end_num = end; 
    }
    inline void set_output_file(const std::string& fn) noexcept { 
        output_file = fn;
    }
public:
    SiteParser(const char* settings_file){
        collect_settings(settings_file);
    }
    void parse_site();
    void print_parse_result();
};

#endif // _SITE_PARSER_H_
