#ifndef _TIME_TICKER_H_
#define _TIME_TICKER_H_

#include "common.h"

class TimeTicker
{
private:
	DWORD m_lastTime;
	BOOL m_isLocked;

public:
	TimeTicker();
	float tick();
	float lock();
	float unlock();
};

#endif
