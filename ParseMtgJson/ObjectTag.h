#pragma once

#include <string>
#include <list>
#include "PointerT.h"

struct ObjectType
{
    enum Type
    {
        Unknown, 

        Comma,
        Period,
		Tap,
		Untap,

        Conjunction,
        ConditionBeginner, 
        Conjunctive,

        CostSeperator, 
        BigSeperator, 
        SelectMarker, 

        Number,
		ManaCost,			// {0} °°ÀÌ Áß°ýÈ£·Î ½ÎÀÎ ¹®Àå

        Color, 
        Keyword, 
        BasicLandType, 
        CardType, 
		QuotedAbility,		// µû¿ÈÇ¥·Î ½ÎÀÎ ¹®Àå

        // µ¿»ç
        Choose, 

        // °áÇÕ ¹®Àå
        ChoiceBegin, 
    };
};

class IObjectTag {
public:
	IObjectTag();
	virtual ~IObjectTag();

    virtual ObjectType::Type GetType() const = 0;
    virtual std::string ToString() const = 0;

    virtual bool IsEqualTo(const IObjectTag* rhs) const = 0;

	inline void AddRef() { ++refCount; }
	inline void RemoveRef() { --refCount; if (refCount < 0) { _CrtDbgBreak(); } }
	inline int GetRefCount() { return refCount; }

private:
	int refCount = 0;
};
typedef PointerT<IObjectTag> ObjectTag;

class IWordTag : public IObjectTag {
public:
    static IWordTag* Create(const std::string& text);
	static IWordTag* Create(const std::string& text, ObjectType::Type type);
};

struct Sentence;

class IChoiceTag : public IObjectTag {
public:
    static IChoiceTag* Create(Sentence& tags);
};

class IConjunctedTag : public IObjectTag {
public:
    static IConjunctedTag* Create(Sentence& tags, ObjectType::Type type);
};
