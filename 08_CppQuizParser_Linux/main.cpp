#include <iostream>
#include "site_parser.h"

int main([[maybe_unused]]int argc, char* argv[]) {
    // argv[1] = "settings.txt"; // for debug only !!!

    if(argv[1] == nullptr){
        std::cerr << "Provide the settings file name after " << argv[0] << '\n';
        return -1;
    }

    SiteParser urp(argv[1]);

    urp.parse_site();
    urp.print_parse_result();
    return 0;
}
