#pragma once
#pragma comment(lib, "winmm.lib")
#include <windows.h>
#include <mmsystem.h>

class FpsClass
{
public:
	FpsClass();
	FpsClass(const FpsClass& other);
	~FpsClass();

	void Initialize();
	void Frame();
	int GetFPS();

private:
	int m_FPS, m_count;
	unsigned long m_startTime;
};

