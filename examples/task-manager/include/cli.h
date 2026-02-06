#pragma once

#include "storage.h"
#include "utils.h"  // Para colores e input
#include <string>

class CLI {
public:
    explicit CLI(Storage& storage);

    void showWelcome() const;
    void run();
    void showHelp() const;

private:
    Storage& storage_;

    // Comandos
    void handleAdd(const std::string& desc);
    void handleList();
    void handleComplete(int id);
    void handleDelete(int id);

    // Parsers (más código)
    std::string getCommand();
    void parseCommand(const std::string& input);
};