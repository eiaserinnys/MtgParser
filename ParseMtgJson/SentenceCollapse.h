#pragma once

#include "ObjectTag.h"

typedef bool (*IsCollapsibleFunctor)(ObjectType::Type type);

extern void Collapse(Sentence& cur, IsCollapsibleFunctor functor, bool toPrint);

extern void CollapseMultipleKeyword(Sentence& cur, bool toPrint);