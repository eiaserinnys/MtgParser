#include "ObjectTag.h"

int g_objectCount = 0;

IObjectTag::IObjectTag()
{
	++g_objectCount;
}

IObjectTag::~IObjectTag()
{
	--g_objectCount;
}