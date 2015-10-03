#include "SentenceCollapse.h"

#include "Literal.h"
#include "Sentence.h"

using namespace std;

extern FILE* debugFile;

//------------------------------------------------------------------------------
static bool IsConnector(ObjectType::Type type)
{
	return
		type == ObjectType::Comma ||
		type == ObjectType::Conjunction;
}

//------------------------------------------------------------------------------
static bool HasCommonTag(ObjectType::Type lhs, ObjectType::Type rhs)
{
	if (lhs == rhs) { return true; }
	if (lhs == ObjectType::Keyword && rhs == ObjectType::QuotedAbility) { return true;  }
	if (lhs == ObjectType::QuotedAbility && rhs == ObjectType::Keyword) { return true; }
	return false;
}

//------------------------------------------------------------------------------
void Collapse(Sentence& cur, IsCollapsibleFunctor functor, bool toPrint)
{
	ObjectType::Type beginType = ObjectType::Unknown;
	Sentence::ContainerType::iterator beginAt = cur.tags.end();
	int count = 0;

	for (auto it = cur.tags.begin(); it != cur.tags.end(); ++it)
	{
		auto type = (*it)->GetType();
		if (beginType != ObjectType::Unknown)
		{
			// 진행중인 시퀀스가 있다
			// 같으면 연결한다
			if (HasCommonTag(beginType, type))
			{
				++count;
				continue;
			}
			else if (IsConnector(type))
			{
				// 연쇄를 끊지는 않는다
				continue;
			}
			else
			{
				// 연쇄가 끝났다
				if (count > 1)
				{
					string prev = cur.ToString();

					// 둘 이상 이어졌다!
					// 중간을 잘라내서 별도 태그로 빼내자
					Sentence mid;
					mid.tags.splice(mid.tags.end(), cur.tags, beginAt, it);

					ObjectTag move;
					if (mid.tags.back()->GetType() == ObjectType::Comma)
					{
						move = mid.tags.back();
						mid.tags.pop_back();
					}

					IConjunctedTag* tag = IConjunctedTag::Create(mid, beginType);
					cur.tags.insert(it, tag);

					if (move.Get() != nullptr)
					{
						cur.tags.insert(it, move);
					}

					if (toPrint)
					{
						string v = cur.ToString();
						fprintf(debugFile, "\"%s\"\t\"%s\"\n", tag->ToString().c_str(), v.c_str());
					}
				}
			}
		}

		// 새로 시작한다
		if (functor(type))
		{
			count = 1;
			beginType = type;
			beginAt = it;
		}
		else
		{
			count = 0;
			beginType = ObjectType::Unknown;
			beginAt = cur.tags.end();
		}
	}
}

//------------------------------------------------------------------------------
struct DualKeyword
{
	string first;
	string second;
	ObjectType::Type type;
};

//------------------------------------------------------------------------------
void CollapseMultipleKeyword(Sentence& cur, bool toPrint)
{
	DualKeyword toCheck[] = {
		DualKeyword{ "first", "strike", ObjectType::Keyword, },
		DualKeyword{ "double", "strike", ObjectType::Keyword, },
		DualKeyword{ "any", "color", ObjectType::Color, },
		DualKeyword{ "artifact", "creature", ObjectType::CardType, },
	};

	for (auto it = cur.tags.begin(); it != cur.tags.end(); )
	{
		auto second = it; ++second;
		if (second != cur.tags.end())
		{
			bool found = false;
			int i = 0;
			for (i = 0; i < COUNT(toCheck); ++i)
			{
				if (_stricmp((*it)->ToString().c_str(), toCheck[i].first.c_str()) == 0 && 
					_stricmp((*second)->ToString().c_str(), toCheck[i].second.c_str()) == 0)
				{
					found = true;
					break;
				}
			}

			if (found)
			{
				IWordTag* tag = IWordTag::Create((*it)->ToString() + " " + (*second)->ToString(), toCheck[i].type);

				auto next = second; ++next;

				cur.tags.erase(it);
				cur.tags.erase(second);

				it = next;

				cur.tags.insert(next, tag);

				if (toPrint)
				{
					string v = cur.ToString();
					fprintf(debugFile, "\"%s\"\t\"%s\"\n", tag->ToString().c_str(), v.c_str());
				}
				continue;
			}
		}

		++it;
	}
}

