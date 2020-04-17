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
	// CPU ����Ŭ�� �󵵸� ���´�.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);

	// QueryPerformance�� �����ϴ��� üũ�Ѵ�.
	if (m_frequency == 0)
		return false;

	// �󵵸� �и�������� ������.
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

void TimerClass::Frame()
{
	INT64 currentTime;
	float timeDifference;

	// ���� �ð� - ���� �ð��� Frequency�� ������.
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;
	m_startTime = currentTime;
}
