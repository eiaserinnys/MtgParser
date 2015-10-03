#include "SentenceParser.h"

#include "Literal.h"
#include "StringUtility.h"
#include "ObjectTag.h"
#include "Sentence.h"

using namespace std;

int maxConditionDepth = 0;

class Parser_ {
public:
    vector<SentenceToPrint>& split;

    bool inChoice = false;
    string chooseCostStr;
    string chooseCondStr;

    SentenceToPrint current;

    ObjectTag choiceTag;
    Sentence curSentence;

    //--------------------------------------------------------------------------
    Parser_(
        vector<SentenceToPrint>& result, 
        const vector<string>& oracle)
        : split(result)
    {
        for (auto it = oracle.begin(); it != oracle.end(); ++it)
        {
            string text = *it;

            // 빌드할 결과물을 리셋
            current = SentenceToPrint();

            // 문장을 단어로 자른다
            curSentence.FromString(text);

            // 액티베이션 코스트를 파싱한다
            current.cost = FindCostString();

            // 선택문 시작인지 확인한다
            if (CheckChoiceBegin()) { continue; }

            // 선택문이면 넣고 아니면 패스
            if (AddChoiceSentence()) { continue; }

            // 일반 문장 처리
            AddSentence();

            // 삭제
            choiceTag.Clear();
            curSentence.Clear();
        }

        if (oracle.empty())
        {
            split.push_back(SentenceToPrint());
        }
     }

    //--------------------------------------------------------------------------
    string FindCostString()
    {
		auto result = curSentence.Find(
			[](IObjectTag* tag) { return tag->GetType() == ObjectType::CostSeperator; });

		if (result.found)
		{
			return curSentence.Splice(result.iterator).ToString();
		}
		return string();
    }

    //--------------------------------------------------------------------------
    bool CheckChoiceBegin()
    {
        choiceTag = IChoiceTag::Create(curSentence);

        if (choiceTag.Get() != nullptr)
        {
            inChoice = true;
            chooseCostStr = current.cost;
            chooseCondStr = choiceTag->ToString();
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------
    bool AddChoiceSentence()
    {
        if (inChoice)
        {
            // 선택문 처리
            if (curSentence.IsFirstTag(ObjectType::SelectMarker))
            {
                curSentence.PopFront();

                current.cost = chooseCostStr;
                current.choice = chooseCondStr;

                AddSentence();
                return true;
            }
            inChoice = false;
        }
        return false;
    }

    //--------------------------------------------------------------------------
    void AddSentence()
    {
        // 조건문 키워드 처리
        current.conditionKeyword = FindReminderKeyword();

        if (!CheckCondition(0))
        {
            AddText();
        }
    }

    //--------------------------------------------------------------------------
    string FindReminderKeyword()
    {
		auto result = curSentence.Find(
			[](IObjectTag* tag) { return tag->GetType() == ObjectType::BigSeperator;  });
		if (result.found)
		{
            Sentence reminder = curSentence.Splice(result.iterator);
            reminder.PopBack();
            return reminder.ToString();
        }
        return string();
    }

    //--------------------------------------------------------------------------
    bool CheckCondition(int index)
    {
        if (index >= sizeof(current.condition) / sizeof(current.condition[0])) 
        { 
            return false;
        }

        // 접속사를 먼저 자르고
        FindConjunction(index);

        // 조건문을 잘라낸다
        if (curSentence.IsFirstTag(ObjectType::ConditionBeginner))
        {
			auto result = curSentence.Find(
				[](IObjectTag* tag) { return tag->GetType() == ObjectType::Comma;  });

			if (result.found)
			{
                Sentence condSentence = curSentence.Splice(result.iterator);
                condSentence.PopBack();
                current.condition[index].condition = condSentence.ToString();

                if (!CheckCondition(index + 1))
                {
                    if (maxConditionDepth < index)
                    {
                        maxConditionDepth = index;
                    }
                    AddText();
                }
                return true;
            }
        }
        return false;
    }

    //--------------------------------------------------------------------------
    void FindConjunction(int index)
    {
        if (curSentence.IsFirstTag(ObjectType::Conjunctive))
        {
            // 쉼표가 있으면 같이 잘라낸다
            auto it = curSentence.tags.begin();
            auto next = curSentence.tags.begin();
            ++next;
            if (next != curSentence.tags.end() && 
                (*next)->GetType() == ObjectType::Comma)
            {
                it = next;
            }
            current.condition[index].conjunction = curSentence.Splice(it).ToString();

            // 임시
            current.condition[index].conjunction += " ";
        }
    }

    //--------------------------------------------------------------------------
    void AddText()
    {
        current.text = curSentence.ToString();
        split.push_back(current);
    }
};

//------------------------------------------------------------------------------
void SentenceParser::Parse(
    vector<SentenceToPrint>& result, 
    const vector<string>& oracle)
{
    Parser_ parser(result, oracle);
}