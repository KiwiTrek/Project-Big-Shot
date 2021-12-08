#ifndef __CONSOLE_BUFFER_H__
#define __CONSOLE_BUFFER_H__

#include "Warnings.h"

struct ConsoleBuffer
{
public:
	ConsoleBuffer()
	{
		initBuff = nullptr;
		initBuff2 = nullptr;
	}

public:
	const char* initBuff;
	const char* initBuff2;
};

#endif // !__CONSOLE_BUFFER_H__