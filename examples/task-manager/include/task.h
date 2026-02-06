#pragma once

#include <string>

class Task {
public:
    // Constructores
    Task();
    Task(int id, const std::string& desc, bool comp = false);

    // Getters
    int getId() const;
    std::string getDescription() const;
    bool isCompleted() const;

    // Setters con validaciones
    void setDescription(const std::string& desc);
    void setCompleted(bool comp);

    // Métodos adicionales (para más código)
    std::string toString() const;
    bool validate() const;  // Chequea si descripción no está vacía

private:
    int id_;
    std::string description_;
    bool completed_;
};