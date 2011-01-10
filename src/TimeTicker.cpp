#include "TimeTicker.h"

TimeTicker::TimeTicker()
{
	m_lastTime = 0;
	m_isLocked = false;
}

float TimeTicker::tick()
{
	DWORD currentTime = GetTickCount();
	float dt = (m_lastTime == 0) ? 0.0f : (currentTime - m_lastTime) / 1000.0f;
	if (!m_isLocked) {
		m_lastTime = currentTime;
	}
	return dt;
}

float TimeTicker::lock()
{
	m_isLocked = false;
	float dt = tick();
	m_isLocked = true;
	return dt;
}

float TimeTicker::unlock()
{
	m_isLocked = false;
	return tick();
}
