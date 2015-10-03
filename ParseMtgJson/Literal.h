#pragma once

#define HYPEN "\xe2\x80\x94"
#define SELECTOR "\xe2\x80\xa2"

#define COUNT(a) (sizeof(a) / sizeof(a[0]))

#define FIND_IN_ARRAY(t) \
    for (int i = 0; i < COUNT(t); ++i) \
    { \
        if (_stricmp(text.c_str(), t[i].c_str()) == 0) { return true; } \
    }
