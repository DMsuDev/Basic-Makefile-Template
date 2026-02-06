#include "utils.h"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace Utils {

void printColored(const std::string& text, const std::string& color) {
#ifdef _WIN32
    // En Windows viejo, ignorar colores o usar WinAPI (pero para simpleza, print normal)
    (void)color;  // Silence unused parameter warning
    std::cout << text;
#else
    std::cout << color << text << RESET;
#endif
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

bool confirm(const std::string& question) {
    printColored(question + " (y/n): ", YELLOW);
    std::string response;
    std::getline(std::cin, response);
    return !response.empty() && std::tolower(response[0]) == 'y';
}

}  // namespace Utils