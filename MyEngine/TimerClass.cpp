#include "TimerClass.h"



TimerClass::TimerClass()
{
}

TimerClass::TimerClass(const TimerClass & other)
{
}


TimerClass::~TimerClass()
{
}

bool TimerClass::Initialize()
{
	// CPU 싸이클의 빈도를 얻어온다.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);

	// QueryPerformance를 지원하는지 체크한다.
	if (m_frequency == 0)
		return false;

	// 빈도를 밀리세컨드로 나눈다.
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

void TimerClass::Frame()
{
	INT64 currentTime;
	float timeDifference;

	// 현재 시간 - 직전 시간에 Frequency를 나눈다.
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;
	m_startTime = currentTime;
}
