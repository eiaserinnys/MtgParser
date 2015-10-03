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

        // 메인 타입과 서브 타입을 자른다
        ParseTypeString();

        // 카드 이름을 엑셀 출력 가능하게 수정한다
        ProcessCardName();

        // 오라클 텍스트를 엑셀 출력 가능하게 수정한다
        ProcessOracleText();

        // 가공을 위해서 텍스트를 먼저 줄 단위로 나눈다
        vector<string> textByLine;
        splitOracleText(textByLine, text);

        if (!textByLine.empty() && textByLine[0].find("Level up") == 0)
        {
            // 레벨업 크리처는 더 이상 가공하지 않는다
            SentenceToPrint sentence;
            sentence.text = text;
            sentences.push_back(sentence);
        }
        else
        {
            // 키워드나 간단한 패시브 능력은 왼편으로 옮긴다
            ParseKeyword(keywords, textByLine, mainType);

            // 소트 가능하도록 코스트를 뒤집는다
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

        // 카드명에서도 "를 찾아서 교체
        replace(name, "\"", "\"\"");
    }

    //--------------------------------------------------------------------------
    void ProcessOracleText()
    {
        text = orgText;

        // 텍스트에서 카드명을 찾아서 교체한다
        replace(text, name, "THIS");

        // 레전더리 크리처인 경우, 
        // 이름 - 수식어구에서 이름만 카드 텍스트에 등장하기도 한다
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

        // 쿼테이션 마크를 교체한다
        replace(text, "\"", "\"\"");

        // 영문 문장 부호 표기를 수정한다
        replace(text, ",\"\"", "\"\",");
        replace(text, ".\"\"", "\"\".");

        // 빈칸 에러를 수정한다
        replace(text, "  ", " ");
    }

    //--------------------------------------------------------------------------
    static void splitOracleText(vector<string>& toReturn, const string& text_)
    {
        vector<string> firstSplit;
        splitOutOfQuote(firstSplit, text_, "\n");

        // 먼저 리마인더를 제외한다
        removeReminderText(firstSplit);

        // 문장을 마침표 단위로 자른다
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
                // 셀렉터 마크가 있는 경우에는 자르지 않는다
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
                // 전체 문장을 생략
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

            // 일종의 키워드처럼 동작하는 문장은 축약한다
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
                // 간혹 세미콜론으로 분리된 오라클 텍스트가 있다
                // 먼저 세미콜론으로 나눠본다
                split(parsed, text, "; ");

                if (parsed.size() <= 1)
                {
                    // 쉼표로 나눈다
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
                // 키워드를 분리 수거한다
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
                // 키워드가 아닌 문장은 남긴다
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