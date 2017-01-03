#pragma once
//#include "stdafx.h"
#include <WinBase.h>

inline void debugPrintf(const char* format, ...)
{
	char buf[1024] = { 0 };
	va_list ap;

	va_start(ap, format);
	vsprintf_s(buf, _countof(buf), format, ap);
	OutputDebugStringA(buf);
	va_end(ap);
}