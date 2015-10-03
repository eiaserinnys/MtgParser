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

            // ������ ������� ����
            current = SentenceToPrint();

            // ������ �ܾ�� �ڸ���
            curSentence.FromString(text);

            // ��Ƽ���̼� �ڽ�Ʈ�� �Ľ��Ѵ�
            current.cost = FindCostString();

            // ���ù� �������� Ȯ���Ѵ�
            if (CheckChoiceBegin()) { continue; }

            // ���ù��̸� �ְ� �ƴϸ� �н�
            if (AddChoiceSentence()) { continue; }

            // �Ϲ� ���� ó��
            AddSentence();

            // ����
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
            // ���ù� ó��
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
        // ���ǹ� Ű���� ó��
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

        // ���ӻ縦 ���� �ڸ���
        FindConjunction(index);

        // ���ǹ��� �߶󳽴�
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
            // ��ǥ�� ������ ���� �߶󳽴�
            auto it = curSentence.tags.begin();
            auto next = curSentence.tags.begin();
            ++next;
            if (next != curSentence.tags.end() && 
                (*next)->GetType() == ObjectType::Comma)
            {
                it = next;
            }
            current.condition[index].conjunction = curSentence.Splice(it).ToString();

            // �ӽ�
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