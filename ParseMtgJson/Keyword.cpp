#include "Keyword.h"
#include "Literal.h"

using namespace std;

static string costKeywords[] = 
    {
        "flashback {", "flashback" HYPEN, 
        "Entwine {", "Entwine" HYPEN, 
        "Equip {", "Equip" HYPEN, 
        "Buyback {", "Buyback" HYPEN, 
        "Cumulative upkeep {", "Cumulative upkeep" HYPEN, 
        "Cycling {", "Cycling" HYPEN, 
        "Dash {", "Dash" HYPEN, 
        "Echo {", "Echo" HYPEN, 
        "Evoke {", "Evoke" HYPEN, 
        "fortify {", "fortify" HYPEN, 
        "kicker {", "kicker" HYPEN, 
        "multikicker {", "multikicker" HYPEN, 
        "madness {", "madness" HYPEN, 
        "megamorph {", "megamorph" HYPEN, 
        "morph {", "morph" HYPEN, 
        "miracle {", "miracle" HYPEN, 
        "ninjutsu {", "ninjutsu" HYPEN, 
        "outblast {", "outblast" HYPEN, 
        "overload {", "overload" HYPEN, 
        "plainscycling {", "plainscycling" HYPEN, 
        "islandcycling {", "islandcycling" HYPEN, 
        "swampcycling {", "swampcycling" HYPEN, 
        "mountaincycling {", "mountaincycling" HYPEN, 
        "forestcycling {", "forestcycling" HYPEN, 
        "wizardcycling {", "wizardcycling" HYPEN, 
        "slivercycling {", "slivercycling" HYPEN, 
        "prowl {", "prowl" HYPEN, 
        "replicate {", "replicate" HYPEN, 
        "scavenge {", "scavange" HYPEN, 
        "transmute {", "transmute" HYPEN, 
        "recover {", "recover" HYPEN, 
        "splice onto arcane {", "splice onto arcane" HYPEN, 
        "unearth {", "unearth" HYPEN, 
        "transfigure {", "transfigure" HYPEN, 
        "outlast {", "outlast" HYPEN, 
    };
static int costKeywordCount = sizeof(costKeywords) / sizeof(costKeywords[0]);

//------------------------------------------------------------------------------
bool isSingleLineKeyword(const string& text)
{
    // ex. Garza's Assassin, Recover—Pay half your life, rounded up.
    if (text.rfind(".") == text.length() - 1)
    {
        for (int i = 0; i < costKeywordCount; ++i)
        {
            if (_strnicmp(text.c_str(), costKeywords[i].c_str(), costKeywords[i].length()) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool isSingleWordKeyword(const string& toTest)
{
    string keywords[] = 
    {
        "deathtouch", "defender", "first strike", "double strike", 
        //"enchant"
        //"equip",
        "flash", "flying", "haste", "hexproof", "indestructible", "intimidate",
        "lifelink", "reach", "scry", "shroud", "trample", "vigilance", 
        "banding", "battle cry", "cascade", "Changeling", "Cipher", "Conspire", 
        "convoke", 
        "plainswalk", "islandwalk", "swampwalk", "mountainwalk", "forestwalk", 
        "Legendary landwalk", "desertwalk", 
        "flanking", "Exploit", "Extort", "Evolve", "Exalted", "Epic", 
        "delve", "fuse", "haunt", "infect", 
        "living weapon", "menace", "persist", "phasing", 
        "fox offering", "moonfolk offering", "rat offering", "snake offering", "goblin offering", 
        "rebound", 
        "provoke", "prowess", "retrace", "shadow", "soulbond", 
        "split second", "storm", "sunburst", "totem armor", "undying", "unleash", 
        "wither", "hideaway", "horsemanship", "gravestorm",
        "dethrone", "Fear", 
		
		"landwalk",
    };
    int count = sizeof(keywords) / sizeof(string);

    for (int i = 0; i < count; ++i)
    {
        if (_stricmp(toTest.c_str(), keywords[i].c_str()) == 0)
        { 
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool isKeyword(const string& toTest)
{
    if (isSingleWordKeyword(toTest))
    {
        return true;
    }

    static string partialKeywords[] = 
    {
        "Affinity for", "Basic landcycling", "Bestow", "Aura swap", "Bloodthirst",
        "Bushido", "Amplify", "Annihilator", 
        "Enchant ", 
        "Fading ", "Protection from ", 
        "Dredge", "modular", "graft", "renown", 
        "soulshift", "suspend", "rampage", "tribute", 
        "Champion a", "vanishing", "reinforce ", "Devour ", "Ripple ", 
    };
    int pcount = sizeof(partialKeywords) / sizeof(string);

    for (int i = 0; i < pcount; ++i)
    {
        if (_strnicmp(toTest.c_str(), partialKeywords[i].c_str(), partialKeywords[i].length()) == 0)
        {
            return true;
        }
    }

    for (int i = 0; i < costKeywordCount; ++i)
    {
        if (_strnicmp(toTest.c_str(), costKeywords[i].c_str(), costKeywords[i].length()) == 0)
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
bool isPseudoKeyword(string& toTest, const string& type)
{
    static pair<string, string> pseudo[] = 
    {
        make_pair("THIS enters the battlefield tapped.", "LAZY"),
        make_pair("THIS attacks each turn if able.", "BERSERK"), 
        make_pair("THIS attacks each combat if able.", "BERSERK"), 
        make_pair("THIS can block only creatures with flying.", "HIGHFLYING"), 
        make_pair("THIS can't attack unless defending player controls an Island.", "ISLANDHOME_A"), 
        make_pair("When you control no Islands, sacrifice THIS.", "ISLANDHOME_B"), 
        make_pair("THIS can't block.", "COWARD"),
        make_pair("THIS can't be countered.", "UNCOUNTABLE"),
        make_pair("THIS can't be countered by spells or abilities.", "UNCOUNTABLE"),
        make_pair("THIS can't be blocked.", "UNBLOCKABLE"), 
        make_pair("Draw a card at the beginning of the next turn's upkeep.", "SLOWCANTRIP"), 
        make_pair("At the beginning of each upkeep, if a player cast two or more spells last turn, transform THIS.", "TO_HUMAN"), 
        make_pair("At the beginning of each upkeep, if no spells were cast last turn, transform THIS.", "TO_WEREWOLF"), 
        make_pair("All creatures able to block THIS do so.", "LURE"), 
        make_pair("When THIS becomes the target of a spell or ability, sacrifice it.", "PHANTASMAL"), 
        make_pair("When THIS is put into a graveyard from anywhere, shuffle it into its owner's library.", "REINCARNATION"), 
        make_pair("When THIS enters the battlefield, draw a card.", "CANTRIP_ETB"), 
        make_pair("You may choose not to untap THIS during your untap step.", "MAYTAPPED"), 
        make_pair("THIS doesn't untap during your untap step.", "STAYTAPPED"), 
        make_pair("Whenever THIS deals damage, you gain that much life.", "OLD_LIFELINK"), 
        make_pair("THIS has indestructible as long as it has a divinity counter on it.", "DIVINITY"), 
        make_pair("As THIS enters the battlefield, choose a color.", "ETB_CHOICE " HYPEN" Color"), 
    };
    int scount = sizeof(pseudo) / sizeof(pseudo[0]);
    for (int i = 0; i < scount; ++i)
    {
        if (_stricmp(toTest.c_str(), pseudo[i].first.c_str()) == 0) { toTest = pseudo[i].second; return true; }
    }

    {
        static string text = "At the beginning of your upkeep, sacrifice THIS unless you ";
        if (toTest.find(text) == 0)
        {
            toTest = "UPKEEP " HYPEN" " + toTest.substr(text.length());
            return true;
        }
    }

    {
        static string text = "As an additional cost to cast THIS, ";
        if (toTest.find(text) == 0)
        {
            toTest = "OFFERING " HYPEN" " + toTest.substr(text.length());
            return true;
        }
    }

    {
        static string text = "Cast THIS only ";
        if (toTest.find(text) == 0)
        {
            toTest = "CAST_ONLY " HYPEN" " + toTest.substr(text.length());
            return true;
        }
    }

    if (type == "Instant" || type == "Sorcery")
    {
        if (toTest == "Draw a card.")
        {
            toTest = "CANTRIP";
            return true;
        }
    }

    return false;
}
