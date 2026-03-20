#pragma once

#include <vector>
#include "task.h"
#include <filesystem>
#include <nlohmann/json.hpp> 

/// Storage class for persistent Task management using JSON
class Storage {
public:
    Storage();
    explicit Storage(const std::string& filename);

    // Add task with auto-increment ID
    void addTask(const std::string& description);
    std::vector<Task> getAllTasks() const;
    void completeTask(int id);
    void deleteTask(int id);

    // Persistence
    void save() const;
    void load();

    // Utilities
    bool exists() const;
    size_t getTaskCount() const;
    Task findTaskById(int id) const;

private:
    std::filesystem::path filePath_;
    std::vector<Task> tasks_;
    int nextId_;

    // Helpers
    void initialize();
    nlohmann::json toJson() const;
    void fromJson(const nlohmann::json& j);
};