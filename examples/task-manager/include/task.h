#pragma once

#include <string>

class Task {
public:
    Task();
    Task(int id, const std::string& desc, bool comp = false);

    int getId() const;
    std::string getDescription() const;
    bool isCompleted() const;

    void setDescription(const std::string& desc);
    void setCompleted(bool comp);

    std::string toString() const;
    bool validate() const;

private:
    int id_;
    std::string description_;
    bool completed_;
};