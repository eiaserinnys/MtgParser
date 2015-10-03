#include "ObjectTag.h"

#include "Literal.h"
#include "Sentence.h"

using namespace std;

class ChoiceBegin : public IChoiceTag {
public:
    Sentence sentence;
    ChoiceBegin(Sentence& src) { sentence = src; }
    ObjectType::Type GetType() const override { return ObjectType::ChoiceBegin; }
    string ToString() const override { return sentence.ToString(); }
    bool IsEqualTo(const IObjectTag* rhs_) const
    { 
        auto rhs = dynamic_cast<const ChoiceBegin*>(rhs_);
        if (rhs != nullptr) { return sentence.IsEqualTo(rhs->sentence); }
        return false;
    }
};

class KahnsOrDragons : public IChoiceTag {
public:
    ObjectType::Type GetType() const override { return ObjectType::ChoiceBegin; }
    string ToString() const override { return "KHANS_OR_DRAGONS"; }
    bool IsEqualTo(const IObjectTag* rhs) const { return dynamic_cast<const KahnsOrDragons*>(rhs) != nullptr; }
};

class OpponentChoose : public IChoiceTag {
public:
    ObjectType::Type GetType() const override { return ObjectType::ChoiceBegin; }
    string ToString() const override { return "OPP_CHOICE"; }
    bool IsEqualTo(const IObjectTag* rhs) const { return dynamic_cast<const OpponentChoose*>(rhs) != nullptr; }
};

IChoiceTag* IChoiceTag::Create(Sentence& sentence)
{
    static Sentence kahnsOrDragons("As THIS enters the battlefield, choose Khans or Dragons.");
    static Sentence opponentChoose("An opponent chooses one " HYPEN);

    if (kahnsOrDragons.IsEqualTo(sentence))
    {
        sentence.Clear();
        return new KahnsOrDragons;
    }

    if (opponentChoose.IsEqualTo(sentence))
    {
        sentence.Clear();
        return new OpponentChoose;
    }

	auto first = sentence.Find(
		[](IObjectTag* tag) -> bool { return tag->GetType() == ObjectType::Choose; });

	if (first.found)
	{
		auto second = sentence.Find(
			[](IObjectTag* tag) -> bool { return tag->GetType() == ObjectType::BigSeperator; }, 
			first.iterator);

		if (second.found)
		{
			return new ChoiceBegin(sentence.Splice(second.iterator));
		}
	}
    return nullptr;
}
