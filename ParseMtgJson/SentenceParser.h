#pragma once

#include <string>
#include <vector>

struct SentenceToPrint
{
    std::string cost;
    std::string choice;
    std::string conditionKeyword;
    struct Condition
    {
        std::string conjunction;
        std::string condition;
    };
    Condition condition[5];
    std::string text;
};

class SentenceParser {
public:
    static void     Parse(
                        std::vector<SentenceToPrint>& result, 
                        const std::vector<std::string>& oracle);
};