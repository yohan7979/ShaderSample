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
	// �� �����Ӹ��� ȣ��Ǿ� count�� ����
	m_count++;

	// 1�ʰ� ������ FPS�� count ����
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
