#include "Sentence.h"

#include <list>
#include <vector>
#include "Literal.h"
#include "StringUtility.h"
#include "ObjectTag.h"
#include "SentenceCollapse.h"

using namespace std;

Sentence::Sentence()
{
}

Sentence::Sentence(const string& text)
{
    FromString(text);
}

Sentence::Sentence(const Sentence& rhs)
{
	operator = (rhs);
}

Sentence& Sentence::operator = (const Sentence& rhs)
{
	Clear();
	tags = rhs.tags;
	return *this;
}

Sentence::~Sentence()
{
    Clear();
}

void Sentence::Clear()
	{ tags.clear(); }

void Sentence::FromString(const string& text)
{
    Clear();

    vector<string> words;
    splitOutOfQuote(words, text, " ");

    vector<string> words2;
    for (auto i = words.begin(); i != words.end(); ++i)
    {
        if (i->empty()) { continue; }

        if (*(i->rbegin()) == '.' || *(i->rbegin()) == ',' || *(i->rbegin()) == ':')
        {
            words2.push_back((*i).substr(0, i->length() - 1));
            words2.push_back((*i).substr(i->length() - 1));
        }
        else
        {
            words2.push_back(*i);
        }
    }

    for (auto i = words2.begin(); i != words2.end(); ++i)
    {
        tags.push_back(IWordTag::Create(*i));
    }

    Collapse();
}

static bool IsColor(ObjectType::Type type)
{
	return type == ObjectType::Color;
}

static bool IsCollapsible(ObjectType::Type type)
{
    return
        type != ObjectType::Unknown &&
        type != ObjectType::ConditionBeginner &&
        type != ObjectType::Conjunction &&
        type != ObjectType::Conjunctive &&
        type != ObjectType::CostSeperator &&
        type != ObjectType::BigSeperator &&
        type != ObjectType::SelectMarker;
        //type == ObjectType::Color || 
        //type == ObjectType::Number ||
        //type == ObjectType::BasicLandType;
}

void Sentence::Collapse()
{
	// 먼저 색깔만 합친다
	::Collapse(*this, IsColor, true);

	// 멀티플 키워드를 묶는다
	::CollapseMultipleKeyword(*this, true);

	// 먼저 색깔만 합친다
	::Collapse(*this, IsCollapsible, true);
}

string Sentence::ToString() const
{
    string t;
    for (auto it = tags.begin(); it != tags.end(); ++it)
    {
        if (it != tags.begin() && 
            (*it)->GetType() != ObjectType::Comma &&
            (*it)->GetType() != ObjectType::Period &&
            (*it)->GetType() != ObjectType::CostSeperator)
        {
            t += " ";
        }
        t += (*it)->ToString();
    }
    return t;
}

bool Sentence::IsEqualTo(const Sentence& rhs) const
{
    ContainerType::const_iterator l = tags.begin(), r = rhs.tags.begin();

    for (; l != tags.end() && r != rhs.tags.end(); ++l, r++)
    {
        if (!(*l)->IsEqualTo(r->Get())) { return false; }
    }

    return l == tags.end() && r == rhs.tags.end();
}

Sentence Sentence::Splice(ContainerType::iterator it)
{
	Sentence toRet;
	Splice(toRet, it);
	return toRet;
}

void Sentence::Splice(Sentence& left, ContainerType::iterator it)
{
    left.tags.splice(left.tags.end(), tags, tags.begin(), ++it);
}

bool Sentence::IsFirstTag(ObjectType::Type type) const
{
    return !tags.empty() && (*tags.begin())->GetType() == type;
}

void Sentence::PopFront()
{
    if (!tags.empty()) { tags.pop_front(); }
}

void Sentence::PopBack()
{
    if (!tags.empty()) { tags.pop_back(); }
}