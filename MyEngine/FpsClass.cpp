#include "FpsClass.h"



FpsClass::FpsClass()
{
}

FpsClass::FpsClass(const FpsClass & other)
{
}


FpsClass::~FpsClass()
{
}

void FpsClass::Initialize()
{
	m_FPS = 0;
	m_count = 0;
	m_startTime = timeGetTime();
}

void FpsClass::Frame()
{
	// 매 프레임마다 호출되어 count를 증가
	m_count++;

	// 1초가 지나면 FPS에 count 저장
	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_FPS = m_count;
		m_count = 0;
		m_startTime = timeGetTime();
	}
}

int FpsClass::GetFPS()
{
	return m_FPS;
}
