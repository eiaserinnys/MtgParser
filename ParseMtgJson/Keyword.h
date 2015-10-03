#pragma once

#include <string>

extern bool isKeyword(const std::string& toTest);

extern bool isSingleWordKeyword(const std::string& toTest);

extern bool isSingleLineKeyword(const std::string& toTest);

extern bool isPseudoKeyword(std::string& toTest, const std::string& type);