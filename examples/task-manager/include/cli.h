#pragma once

#include "storage.h"
#include "utils.h"
#include <string>

class CLI {
public:
    explicit CLI(Storage& storage);

    void showWelcome() const;
    void run();
    void showHelp() const;

private:
    Storage& storage_;

    void handleAdd(const std::string& desc);
    void handleList();
    void handleComplete(int id);
    void handleDelete(int id);

    std::string getCommand();
    void parseCommand(const std::string& input);
};