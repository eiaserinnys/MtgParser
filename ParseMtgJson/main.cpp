#include <stdio.h>
#include <rapidjson.h>
#include <document.h>

#include "Card.h"

using namespace std;
using namespace rapidjson;

FILE* csvFile = nullptr;

FILE* debugFile = nullptr;

extern int maxConditionDepth;

int main()
{
    FILE* jsonFile = nullptr;
    fopen_s(&jsonFile, "AllCards-x.json", "r");

    fopen_s(&csvFile, "mtg.csv", "w");

    fopen_s(&debugFile, "debug.txt", "w");

    if (jsonFile != nullptr && csvFile != nullptr && debugFile != nullptr)
    {
        fseek(jsonFile, 0, SEEK_END);
        size_t sz = ftell(jsonFile);
        fseek(jsonFile, 0, SEEK_SET);

        // 파일을 통째로 읽는다
        char* buffer = new char[sz + 1];
        fread(buffer, sizeof(char), sz, jsonFile);
        buffer[sz] = 0;

        int count = 0;

        // JSON으로 파싱한다
        Document document;
        if (document.Parse(buffer).HasParseError())
        {
        }
        else
        {
            // BOM을 쓴다
            unsigned char bom[] = { 0xef, 0xbb, 0xbf, 0 };
            fprintf(csvFile, "%s", bom);

            // 모든 카드를 순회한다
            for (auto it = document.MemberBegin(); it != document.MemberEnd(); ++it)
            {
                auto& v = it->value;

                // 이름
                string name = v["name"].GetString();

                // 커맨더 사용 가능인지 확인한다
                if (v.HasMember("legalities"))
                {
                    bool legal = false;
                    auto& legalities = v["legalities"];
                    for (auto lit = legalities.Begin(); lit != legalities.End(); lit++)
                    {
                        auto& l = *lit;
                        auto& a = l["format"];
                        auto& b = l["legality"];
                        if (_stricmp(a.GetString(), "Commander") == 0 &&
                            _stricmp(b.GetString(), "Legal") == 0)
                        {
                            legal = true;
                            break;
                        }
                    }

                    if (!legal)
                    {
                        continue;
                    }
                }
                else
                {
                    continue;
                }

                // 마나코스트
                string manaCost;
                if (v.HasMember("manaCost")) { manaCost = v["manaCost"].GetString(); }

                // 타입
                string type;
                if (v.HasMember("type")) { type = v["type"].GetString(); }

                // 오라클 텍스트
                string text, orgText;
                if (v.HasMember("text")) { orgText = text = v["text"].GetString(); }

                auto card = ICard::create(name, type, manaCost, text);
                delete card;

                count++;
            }
       }

        printf(
            "total %d cards processed, condition depth %d\n", 
            count, 
            maxConditionDepth);

        delete[] buffer;
    }

    if (jsonFile != nullptr) { fclose(jsonFile); }
    if (csvFile != nullptr) { fclose(csvFile); }
    if (debugFile != nullptr) { fclose(debugFile); }
    
    return 0;
}