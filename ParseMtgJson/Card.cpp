#include "Card.h"

#include "Literal.h"
#include "StringUtility.h"
#include "Keyword.h"
#include "SentenceParser.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern FILE* csvFile;

extern int g_objectCount;

class Card : public ICard {
public:
    string orgName, orgType, manaCost, orgText;

    string text;

    string mainType, subType;
    string name;

    vector<string> keywords;

    vector<SentenceToPrint> sentences;

    //--------------------------------------------------------------------------
    Card(
        const string& name_, 
        const string& type_, 
        const string& manaCost_,
        const string& text_)
        : orgName(name_)
        , orgType(type_)
        , manaCost(manaCost_)
        , orgText(text_)
    {
        if (name_ != "Quirion Dryad")
        {
            //return;
        }

        // ���� Ÿ�԰� ���� Ÿ���� �ڸ���
        ParseTypeString();

        // ī�� �̸��� ���� ��� �����ϰ� �����Ѵ�
        ProcessCardName();

        // ����Ŭ �ؽ�Ʈ�� ���� ��� �����ϰ� �����Ѵ�
        ProcessOracleText();

        // ������ ���ؼ� �ؽ�Ʈ�� ���� �� ������ ������
        vector<string> textByLine;
        splitOracleText(textByLine, text);

        if (!textByLine.empty() && textByLine[0].find("Level up") == 0)
        {
            // ������ ũ��ó�� �� �̻� �������� �ʴ´�
            SentenceToPrint sentence;
            sentence.text = text;
            sentences.push_back(sentence);
        }
        else
        {
            // Ű���峪 ������ �нú� �ɷ��� �������� �ű��
            ParseKeyword(keywords, textByLine, mainType);

            // ��Ʈ �����ϵ��� �ڽ�Ʈ�� �����´�
			{
				int prev = g_objectCount;

				SentenceParser::Parse(sentences, textByLine);

				if (g_objectCount > prev)
				{
					int i = 0;
				}
			}
        }

        if (!sentences.empty())
        {
            for (auto it = sentences.begin(); it != sentences.end(); ++it)
            {
                fprintf(csvFile, 
                    "\"%s\",\"%s\",\"%s\",\"",
                    name.c_str(), 
                    manaCost.c_str(), 
                    mainType.c_str());

                for (auto kit = keywords.begin(); kit != keywords.end(); ++kit)
                {
                    fprintf(csvFile, "%s", (*kit).c_str());
                    if (kit + 1 != keywords.end()) { fprintf(csvFile, "\n"); }
                }

                fprintf(csvFile, 
                    "\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
                    (*it).cost.c_str(),
                    (*it).choice.c_str(),
                    (*it).conditionKeyword.c_str(),
                    (*it).condition[0].conjunction.c_str(),
                    (*it).condition[0].condition.c_str(), 
                    (*it).condition[1].conjunction.c_str(),
                    (*it).condition[1].condition.c_str(), 
                    (*it).text.c_str());
            }
        }
    }

    //--------------------------------------------------------------------------
    void ParseTypeString()
    {
        static string seperator = " " HYPEN " ";
        size_t sep = orgType.find(seperator);
        if (sep != string::npos)
        { 
            mainType = orgType.substr(0, sep);
            subType = orgType.substr(sep + seperator.length());
        }
        else
        {
            mainType = orgType;
        }
    }

    //--------------------------------------------------------------------------
    void ProcessCardName()
    {
        name = orgName;

        // ī������� "�� ã�Ƽ� ��ü
        replace(name, "\"", "\"\"");
    }

    //--------------------------------------------------------------------------
    void ProcessOracleText()
    {
        text = orgText;

        // �ؽ�Ʈ���� ī����� ã�Ƽ� ��ü�Ѵ�
        replace(text, name, "THIS");

        // �������� ũ��ó�� ���, 
        // �̸� - ���ľ���� �̸��� ī�� �ؽ�Ʈ�� �����ϱ⵵ �Ѵ�
        if (mainType.find("Legendary") != string::npos && 
            mainType.find("Creature") != string::npos)
        {
            {
                size_t pos = name.find(", ");
                if (pos != string::npos)
                {
                    string subName = name.substr(0, pos);
                    replace(text, subName, "THIS");
                }
            }

            {
                size_t pos = name.find(" ");
                if (pos != string::npos)
                {
                    string subName = name.substr(0, pos);
                    replace(text, subName, "THIS");
                }
            }
        }

        // �����̼� ��ũ�� ��ü�Ѵ�
        replace(text, "\"", "\"\"");

        // ���� ���� ��ȣ ǥ�⸦ �����Ѵ�
        replace(text, ",\"\"", "\"\",");
        replace(text, ".\"\"", "\"\".");

        // ��ĭ ������ �����Ѵ�
        replace(text, "  ", " ");
    }

    //--------------------------------------------------------------------------
    static void splitOracleText(vector<string>& toReturn, const string& text_)
    {
        vector<string> firstSplit;
        splitOutOfQuote(firstSplit, text_, "\n");

        // ���� �����δ��� �����Ѵ�
        removeReminderText(firstSplit);

        // ������ ��ħǥ ������ �ڸ���
        for (auto it = firstSplit.begin(); it != firstSplit.end(); ++it)
        {
            if ((*it).find(SELECTOR" ") == string::npos)
            {
                vector<string> secondSplit;
                splitOutOfQuote(secondSplit, *it, ". ");

                for (auto sit = secondSplit.begin(); sit != secondSplit.end(); ++sit)
                {
                    if (sit + 1 != secondSplit.end())
                    {
                        *sit += ".";
                    }
                }

                toReturn.insert(toReturn.end(), secondSplit.begin(), secondSplit.end());
            }
            else
            {
                // ������ ��ũ�� �ִ� ��쿡�� �ڸ��� �ʴ´�
                toReturn.push_back(*it);
            }
        }
    }

    //--------------------------------------------------------------------------
    static void removeReminderText(vector<string>& result)
    {
        vector<string> src = result;
        result.clear();

        for (auto it = src.begin(); it != src.end(); ++it)
        {
            string text = *it;
            size_t left = 0;
            while ((left = text.find(" (")) != string::npos)
            {
                size_t right = text.find(")", left);
                text = text.substr(0, left) + text.substr(right + 1);
             }

            left = text.find("(");
            if (left != string::npos || text.empty())
            {
                // ��ü ������ ����
                continue;
            }

            result.push_back(text);
        }
    }

    //--------------------------------------------------------------------------
    void ParseKeyword(vector<string>& keywords, vector<string>& texts, const string& type)
    {
        vector<string> texts_ = texts;
        texts.clear();

        for (auto it = texts_.begin(); it != texts_.end(); ++it)
        {
            auto& text = *it;

            // ������ Ű����ó�� �����ϴ� ������ ����Ѵ�
            if (isPseudoKeyword(text, type))
            {
                keywords.push_back(text);
                continue;
            }

            bool allKeyword = true;

            vector<string> parsed;

            if (isSingleLineKeyword(text))
            {
                parsed.push_back(text);
            }
            else
            {
                // ��Ȥ �����ݷ����� �и��� ����Ŭ �ؽ�Ʈ�� �ִ�
                // ���� �����ݷ����� ��������
                split(parsed, text, "; ");

                if (parsed.size() <= 1)
                {
                    // ��ǥ�� ������
                    parsed.clear();
                    split(parsed, text, ", ");
                }

                for (auto kit = parsed.begin(); kit != parsed.end(); ++kit)
                {
                    allKeyword = allKeyword && isKeyword(*kit);
                    if (!allKeyword) { break; }
                }
            }

            if (allKeyword)
            {
                // Ű���带 �и� �����Ѵ�
                string aggregated;
                for (auto kit = parsed.begin(); kit != parsed.end(); ++kit)
                {
                    aggregated += (*kit);
                    if (kit + 1 != parsed.end()) { aggregated += ", "; }
                }

                keywords.push_back(aggregated);
            }
            else
            {
                // Ű���尡 �ƴ� ������ �����
                texts.push_back(text);
            }
        }
    }
};

ICard* ICard::create(
    const string& name_, 
    const string& type_, 
    const string& manaCost_,
    const string& text_)
{
    return new Card(name_, type_, manaCost_, text_);
}