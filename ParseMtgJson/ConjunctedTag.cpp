#include "ObjectTag.h"

#include "Sentence.h"

using namespace std;

class ConjunctedTag : public IConjunctedTag {
public:
    Sentence sentence;
    ObjectType::Type type;
    ConjunctedTag(Sentence& src, ObjectType::Type type)
        : type(type) { sentence = src; }
    ObjectType::Type GetType() const override { return type; }
    string ToString() const override { return "(" + sentence.ToString() + ")"; }
    bool IsEqualTo(const IObjectTag* rhs_) const
    { 
        auto rhs = dynamic_cast<const ConjunctedTag*>(rhs_);
        if (rhs != nullptr) { return type == rhs->type && sentence.IsEqualTo(rhs->sentence); }
        return false;
    }
};

IConjunctedTag* 
    IConjunctedTag::Create(Sentence& tags, ObjectType::Type type)
    { return new ConjunctedTag(tags, type); }