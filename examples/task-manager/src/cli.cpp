#include "cli.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

CLI::CLI(Storage& storage) : storage_(storage) {}

void CLI::showWelcome() const {
    Utils::printColored("\n================ Task Manager CLI ================\n", Utils::GREEN);
    Utils::printColored("Welcome! Type ", Utils::GREEN);
    std::cout << "'help'";
    Utils::printColored(" for available commands.\n", Utils::GREEN);
    Utils::printColored("Type 'quit' or 'q' to exit.\n\n", Utils::GREEN);
}

void CLI::run() {
    while (true) {
        std::string input = getCommand();
        if (input == "quit" || input == "q") {
            Utils::printColored("Goodbye!\n", Utils::YELLOW);
            break;
        }
        parseCommand(input);
    }
}

void CLI::showHelp() const {
    std::cout << "Commands:\n";
    std::cout << "  add \"description\"  - Add a new task\n";
    std::cout << "  list                - List all tasks\n";
    std::cout << "  complete <id>       - Mark task as completed\n";
    std::cout << "  delete <id>         - Delete task\n";
    std::cout << "  help                - Show this help\n";
    std::cout << "  quit / q            - Exit\n\n";
}

std::string CLI::getCommand() {
    std::string input;
    Utils::printColored("> ", Utils::BLUE);
    std::getline(std::cin, input);
    return input;
}

void CLI::parseCommand(const std::string& input) {
    std::istringstream iss(input);
    std::string cmd;
    iss >> cmd;

    try {
        if (cmd == "add") {
            std::string desc;
            std::getline(iss, desc);  // Tomar el resto como desc
            if (!desc.empty() && desc[0] == ' ') desc.erase(0, 1);  // Trim space
            handleAdd(desc);
        } else if (cmd == "list") {
            handleList();
        } else if (cmd == "complete") {
            int id;
            iss >> id;
            handleComplete(id);
        } else if (cmd == "delete") {
            int id;
            iss >> id;
            handleDelete(id);
        } else if (cmd == "help") {
            showHelp();
        } else {
            Utils::printColored("Unknown command. Type 'help'.\n", Utils::RED);
        }
    } catch (const std::exception& e) {
        Utils::printColored("Error: " + std::string(e.what()) + "\n", Utils::RED);
    }
}

void CLI::handleAdd(const std::string& desc) {
    if (desc.empty()) {
        throw std::invalid_argument("Description required");
    }
    storage_.addTask(desc);
    Utils::printColored("Task added successfully.\n", Utils::GREEN);
}

void CLI::handleList() {
    auto tasks = storage_.getAllTasks();
    if (tasks.empty()) {
        Utils::printColored("No tasks yet.\n", Utils::YELLOW);
        return;
    }
    
    const int width = 70;
    std::string border(width, '=');
    std::string sep(width, '-');
    
    std::cout << border << "\n";
    std::cout << "| " << std::setw(width-4) << std::left << "TASKS" << " |\n";
    std::cout << sep << "\n";
    
    for (const auto& task : tasks) {
        std::string status = task.isCompleted() ? "[C]" : "[P]";
        std::stringstream line;
        line << "| #" << std::setw(3) << std::right << task.getId() << " " << status << " " << std::left << task.getDescription();
        std::string output = line.str();
        if ((int)output.size() > width - 1) {
            output = output.substr(0, width - 4) + "... |";
        } else {
            output += std::string(width - 1 - output.size(), ' ') + "|";
        }
        std::cout << output << "\n";
    }
    
    std::cout << sep << "\n";
    std::string legend = "| [P]=Pending [C]=Completed";
    legend += std::string(width - legend.size() - 1, ' ') + "|";
    std::cout << legend << "\n";
    std::cout << border << "\n";
}

void CLI::handleComplete(int id) {
    storage_.completeTask(id);
    Utils::printColored("Task " + std::to_string(id) + " completed.\n", Utils::GREEN);
}

void CLI::handleDelete(int id) {
    storage_.deleteTask(id);
    Utils::printColored("Task " + std::to_string(id) + " deleted.\n", Utils::GREEN);
}