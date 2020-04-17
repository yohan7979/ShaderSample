#pragma once
#include <windows.h>

class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass& other);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetTime() { return m_frameTime; }

private:
	INT64 m_frequency;
	INT64 m_startTime;
	float m_ticksPerMs;
	float m_frameTime;
};

