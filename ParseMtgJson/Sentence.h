#pragma once

#include "ObjectTag.h"

struct Sentence {
public:
    typedef std::list<ObjectTag> ContainerType;

    Sentence();
    Sentence(const std::string& text);
	Sentence(const Sentence& rhs);
	~Sentence();

	Sentence& operator = (const Sentence& rhs);

    void FromString(const std::string& text);

    void Collapse();

    bool IsEqualTo(const Sentence& sentence) const;

    void Splice(Sentence& prior, ContainerType::iterator it);

	Sentence Splice(ContainerType::iterator it);

    bool IsFirstTag(ObjectType::Type type) const;

    void PopFront();
    void PopBack();

    void Clear();

    std::string ToString() const;

	struct FindResult
	{
		bool found;
		ContainerType::iterator iterator;
	};

	template <typename MatchFunctor>
	FindResult Find(MatchFunctor functor)
	{
		return Find(functor, tags.begin());
	}

	template <typename MatchFunctor>
	FindResult Find(MatchFunctor functor, ContainerType::iterator it)
	{
		for (; it != tags.end(); ++it)
		{
			if (functor(it->Get())) { return FindResult{ true, it }; }
		}
		return FindResult{ false, it };
	}

	bool IsEnd(ContainerType::iterator it)
	{
		return tags.end() == it;
	}

//private:
    ContainerType tags;
};
