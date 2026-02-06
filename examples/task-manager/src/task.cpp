#include "task.h"
#include <stdexcept>

Task::Task() : id_(0), completed_(false) {}

Task::Task(int id, const std::string& desc, bool comp)
    : id_(id), description_(desc), completed_(comp) {}

int Task::getId() const { return id_; }

std::string Task::getDescription() const { return description_; }

bool Task::isCompleted() const { return completed_; }

void Task::setDescription(const std::string& desc) {
    if (desc.empty()) {
        throw std::invalid_argument("Description cannot be empty");
    }
    description_ = desc;
}

void Task::setCompleted(bool comp) { completed_ = comp; }

std::string Task::toString() const {
    return "ID: " + std::to_string(id_) + " | Desc: " + description_ +
           " | Completed: " + (completed_ ? "Yes" : "No");
}

bool Task::validate() const { return !description_.empty(); }