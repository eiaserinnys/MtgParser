#pragma once

#include <string>
#include <vector>

extern void replace(std::string& text, const std::string& find, const std::string& rep);

extern void split(std::vector<std::string>& toReturn, const std::string& text_, const std::string& seperator);

extern std::size_t findOutOfQuote(const std::string& text, const std::string& toFind, std::size_t begin);

extern void splitOutOfQuote(std::vector<std::string>& toReturn, const std::string& text_, const std::string& seperator);
