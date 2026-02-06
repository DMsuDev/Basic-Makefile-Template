#include <iostream>
#include "cli.h"
#include "storage.h"

int main() {
    // Initialize storage (auto-detects path for executable directory)
    // Falls back to current directory or ~/.taskmanager if write permission denied
    Storage storage;

    // Initialize CLI
    CLI cli(storage);

    // Show welcome message
    cli.showWelcome();

    // Main loop
    cli.run();

    return 0;
}