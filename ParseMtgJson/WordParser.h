#pragma once

#include <string>
#include <list>

class IObjectTag;
struct Sentence;

class WordParser {
public:
    static void Parse(Sentence& sentence, std::string& text);
};