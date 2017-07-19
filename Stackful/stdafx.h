// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#if defined(_WIN32) || defined(_WIN64)
// Required on Windows to build under MinGW, otherwise one receives an error
// in the link stage about multiple definition of `vsnprintf`.
// See: https://stackoverflow.com/questions/43294488/mingw-g-multiple-definition-of-vsnprintf-when-using-to-string
#define __USE_MINGW_ANSI_STDIO 0

#pragma once

#include "targetver.h"

#endif

