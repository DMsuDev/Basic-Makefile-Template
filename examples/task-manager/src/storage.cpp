#include "storage.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <vector>
#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#  include <limits.h>
#endif
#if defined(__APPLE__)
#  include <mach-o/dyld.h>
#endif

using json = nlohmann::json;
namespace fs = std::filesystem;

Storage::Storage() : nextId_(1) {
    // Auto-detect executable directory (Windows/Linux/macOS)
    auto exeDir = fs::path();
#ifdef _WIN32
    char buf[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buf, MAX_PATH);
    if (len != 0) {
        exeDir = fs::path(std::string(buf, static_cast<size_t>(len))).parent_path();
    } else {
        exeDir = fs::current_path();
    }
#else
#if defined(__APPLE__)
    uint32_t size = 0;
    _NSGetExecutablePath(NULL, &size);
    if (size > 0) {
        std::vector<char> buf(size);
        if (_NSGetExecutablePath(buf.data(), &size) == 0) {
            exeDir = fs::path(std::string(buf.data())).parent_path();
        } else {
            exeDir = fs::current_path();
        }
    } else {
        exeDir = fs::current_path();
    }
#else
    char buf[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len != -1) {
        buf[len] = '\0';
        exeDir = fs::path(std::string(buf)).parent_path();
    } else {
        exeDir = fs::current_path();
    }
#endif
#endif
    filePath_ = exeDir / "tasks.json";
    initialize();
}

Storage::Storage(const std::string& filename) : nextId_(1) {
    filePath_ = fs::current_path() / filename;
    initialize();
}

void Storage::initialize() {
    if (!fs::exists(filePath_)) {
        save();
        std::cout << "Created new tasks file: " << filePath_ << std::endl;
    } else {
        load();
    }
}

void Storage::addTask(const std::string& description) {
    if (description.empty()) {
        throw std::invalid_argument("Description cannot be empty");
    }
    Task task(nextId_++, description);
    tasks_.push_back(task);
    save();
}

std::vector<Task> Storage::getAllTasks() const { return tasks_; }

void Storage::completeTask(int id) {
    for (auto& task : tasks_) {
        if (task.getId() == id) {
            task.setCompleted(true);
            save();
            return;
        }
    }
    throw std::runtime_error("Task ID not found");
}

void Storage::deleteTask(int id) {
    auto it = std::remove_if(tasks_.begin(), tasks_.end(),
                             [id](const Task& t) { return t.getId() == id; });
    if (it == tasks_.end()) {
        throw std::runtime_error("Task ID not found");
    }
    tasks_.erase(it, tasks_.end());
    save();
}

void Storage::save() const {
    std::ofstream ofs(filePath_);
    if (!ofs) {
        throw std::runtime_error("Cannot open file for writing: " + filePath_.string());
    }
    ofs << std::setw(4) << toJson() << std::endl;  // Pretty print
}

void Storage::load() {
    std::ifstream ifs(filePath_);
    if (!ifs) {
        throw std::runtime_error("Cannot open file for reading: " + filePath_.string());
    }
    json j;
    ifs >> j;
    fromJson(j);
}

bool Storage::exists() const { return fs::exists(filePath_); }

size_t Storage::getTaskCount() const { return tasks_.size(); }

Task Storage::findTaskById(int id) const {
    for (const auto& task : tasks_) {
        if (task.getId() == id) return task;
    }
    throw std::runtime_error("Task ID not found");
}

json Storage::toJson() const {
    json j = json::array();
    for (const auto& task : tasks_) {
        j.push_back({
            {"id", task.getId()},
            {"description", task.getDescription()},
            {"completed", task.isCompleted()}
        });
    }
    return {{"tasks", j}, {"nextId", nextId_}};  // Guardar nextId para persistencia
}

void Storage::fromJson(const json& j) {
    tasks_.clear();
    if (j.contains("tasks") && j["tasks"].is_array()) {
        for (const auto& item : j["tasks"]) {
            Task t(
                item["id"].get<int>(),
                item["description"].get<std::string>(),
                item["completed"].get<bool>()
            );
            if (t.validate()) {
                tasks_.push_back(t);
            }
        }
    }
    nextId_ = j.contains("nextId") ? j["nextId"].get<int>() : 1;
}