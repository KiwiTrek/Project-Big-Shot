#ifndef __TIMER_H__
#define __TIMER_H__

#include "Warnings.h"
#include "Globals.h"
#include "SDL.h"

// Body for CPU Tick Timer class
class Timer
{
public:
	Timer();

	void Start();
	void Stop();

	uint32 Read() const;
	float ReadSec() const;

private:
	bool running;
	uint32 startedAt;
	uint32 stoppedAt;
};

#endif //__TIMER_H__