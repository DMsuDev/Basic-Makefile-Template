#pragma once

#include <vector>
#include "task.h"
#include <filesystem>  // Cross-platform paths (C++17+)
#include <nlohmann/json.hpp>  // JSON lib header-only

/**
 * @brief Storage class for persistent Task management using JSON
 */
class Storage {
public:
    // Constructor: Finds or creates storage file (next to executable or fallback)
    Storage();
    explicit Storage(const std::string& filename);

    // Add task with auto-increment ID
    void addTask(const std::string& description);
    std::vector<Task> getAllTasks() const;
    void completeTask(int id);
    void deleteTask(int id);

    // Persistencia
    void save() const;
    void load();

    // Utilidades (más código)
    bool exists() const;
    size_t getTaskCount() const;
    Task findTaskById(int id) const;

private:
    std::filesystem::path filePath_;  // Path completo cross-platform
    std::vector<Task> tasks_;
    int nextId_;  // Auto-incremental

    // Helpers
    void initialize();  // Common initialization logic
    nlohmann::json toJson() const;
    void fromJson(const nlohmann::json& j);
};