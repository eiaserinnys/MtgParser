#include "ObjectTag.h"

#include <ctype.h>
#include "Literal.h"
#include "Keyword.h"

using namespace std;

struct WordTag : public IWordTag
{
    WordTag(const string& text)
        : type(ObjectType::Unknown)
        , text(text)
    {
        if (IsColor(text)) { type = ObjectType::Color; }
        else if (IsConditionBeginner(text)) { type = ObjectType::ConditionBeginner; }
        else if (IsConjunction(text)) { type = ObjectType::Conjunction; }
        else if (isSingleWordKeyword(text)) { type = ObjectType::Keyword; }
        else if (IsNumber(text)) { type = ObjectType::Number; }
        else if (IsPeriod(text)) { type = ObjectType::Period; }
        else if (IsComma(text)) { type = ObjectType::Comma; }
        else if (IsCostSeperator(text)) { type = ObjectType::CostSeperator; }
        else if (IsBigSeperator(text)) { type = ObjectType::BigSeperator; }
        else if (IsSelectMarker(text)) { type = ObjectType::SelectMarker; }
        else if (IsChoose(text)) { type = ObjectType::Choose; }
        else if (IsConjunctive(text)) { type = ObjectType::Conjunctive; }
        else if (IsBasicLandType(text)) { type = ObjectType::BasicLandType; }
        else if (IsCardType(text)) { type = ObjectType::CardType; }
		else if (!text.empty() && text[0] == '\"') { type = ObjectType::QuotedAbility; }
		else if (!text.empty() && text[0] == '{')
		{ 
			if (text.length() >= 2 && text[1] == 'T') { type = ObjectType::Tap; }
			else if (text.length() >= 2 && text[1] == 'Q') { type = ObjectType::Untap; }
			else { type = ObjectType::ManaCost; }
		}
	}

	WordTag(const string& text, ObjectType::Type type)
		: type(type)
		, text(text)
	{}

    static bool IsColor(const string& text)
    {
        static string t[] = { 
            "white", "blue", "black", "red", "green", 
        };
        FIND_IN_ARRAY(t);
        return false;
    }

    static bool IsNumber(const string& text)
    {
        static string t[] = {
            "one", "two", "three", "four", "five", 
            "six", "seven", "eight", "nine", "ten", 
            "few", "more", 
        };

        bool allDigit = true;
        for (auto i = text.begin(); i != text.end(); ++i)
        {
            if ((*i) < '0' || '9' < (*i)) { allDigit = false; break; }
        }
        if (allDigit) { return true; }

        FIND_IN_ARRAY(t);
        return false;
    }

    static bool IsConjunction(const string& text)
    {
        static string t[] = {
            "or", "and", "and/or",   // 편의상 이렇게 처리
        };
        FIND_IN_ARRAY(t);
        return false;
    }

    static bool IsConditionBeginner(const string& text)
    {
        static string t[] = {
            "When", "Whenever", "As", "At", "Until",
            "If", "Starting", "After", "Rather", "For", "In", 
        };
        FIND_IN_ARRAY(t);
        return false;
    }

    static bool IsBasicLandType(const string& text)
    {
        static string t[] = {
            "Plains", "Island", "Swamp", "Mountain", "Forest", 
        };
        FIND_IN_ARRAY(t);
        return false;
    }

    static bool IsConjunctive(const string& text)
    {
        // 이후에 쉼표가 이어질 수 있다
        static string t[] = {
            "also", "instead", "then", "simultaneously", "otherwise",
        };
        FIND_IN_ARRAY(t);
        return false;
    }

    static bool IsCardType(const string& text)
    {
        // 이후에 쉼표가 이어질 수 있다
        static string t[] = {
            "artifact", 
            "creature", 
            "enchantment", 
            "instant", 
            "land", 
            "planeswalker", 
            "sorcery", 
            "tribal", 
        };
        FIND_IN_ARRAY(t);
        return false;
    }

    static bool IsChoose(const string& text)
    {
        // 이후에 쉼표가 이어질 수 있다
        static string t[] = {
            "choose",
        };
        FIND_IN_ARRAY(t);
        return false;
    }

    static bool IsPeriod(const string& text)
        { return text == "."; }

    static bool IsComma(const string& text)
        { return text == ","; }

    static bool IsCostSeperator(const string& text)
        { return text == ":"; }

    static bool IsBigSeperator(const string& text)
        { return text == HYPEN; }

    static bool IsSelectMarker(const string& text)
        { return text == SELECTOR; }

    ObjectType::Type GetType() const override { return type; }

    string ToString() const override { return text; }

    bool IsEqualTo(const IObjectTag* rhs_) const override
    {
        auto rhs = dynamic_cast<const WordTag*>(rhs_);
        if (rhs != nullptr)
        {
            return type == rhs->type && text == rhs->text;
        }
        return false;
    }

    ObjectType::Type  type;
    string          text;
};

IWordTag* IWordTag::Create(const string& text)
    { return new WordTag(text); }

IWordTag* IWordTag::Create(const string& text, ObjectType::Type type)
{
	return new WordTag(text, type);
}