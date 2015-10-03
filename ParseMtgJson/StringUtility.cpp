#include "StringUtility.h"
#include "Literal.h"

using namespace std;

void replace(string& text, const string& find, const string& rep)
{
    size_t pos = 0;
    while ((pos = text.find(find, pos)) != string::npos)
    {
        text.replace(pos, find.length(), rep);
        pos += rep.length();
    }                
}

void split(vector<string>& toReturn, const string& text_, const string& seperator)
{
    string text = text_;

    size_t pos = 0;
    while ((pos = text.find(seperator)) != string::npos)
    {
        toReturn.push_back(text.substr(0, pos));
        text = text.substr(pos + seperator.length());
    }
    toReturn.push_back(text);
}

size_t findOutOfQuote(const string& text, const string& toFind, size_t begin = 0)
{
    bool inQuote = false;
    size_t matching = string::npos;
    size_t matchPointer = 0;

    for (size_t i = begin; i < text.length(); )
    {
        if (text[i] == '\"' && (i + 1) < text.length() && text[i + 1] == '\"')
        {
            inQuote = !inQuote;

            matching = string::npos;
            matchPointer = 0;

            i += 2;
            continue;
        }

        if (!inQuote && text[i] == toFind[matchPointer])
        {
            if (matching == string::npos) { matching = i; }
            matchPointer++;
            if (matchPointer == toFind.length()) { return matching; }
        }
        else
        {
            matching = string::npos;
            matchPointer = 0;
        }

        i++;
    }
    return string::npos;
}

void splitOutOfQuote(vector<string>& toReturn, const string& text_, const string& seperator)
{
    string text = text_;

    size_t pos = 0;
    while ((pos = findOutOfQuote(text, seperator)) != string::npos)
    {
        toReturn.push_back(text.substr(0, pos));
        text = text.substr(pos + seperator.length());
    }
    toReturn.push_back(text);
}

