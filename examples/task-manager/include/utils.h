#pragma once

#include <string>

namespace Utils {

// Colores ANSI (cross-platform en terminals modernas; en Windows 10+ ok)
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string CYAN = "\033[36m";

// Función para print con color
void printColored(const std::string& text, const std::string& color);

// Otras utils (más código)
std::string trim(const std::string& str);
bool confirm(const std::string& question);

}  // namespace Utils