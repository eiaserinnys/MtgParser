#pragma once

#include <string>

class ICard {
public:
    virtual ~ICard() {}

    static ICard* create(
        const std::string& name, 
        const std::string& type, 
        const std::string& cost,
        const std::string& text);
};