#pragma once
#include <windows.h>

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass& other);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	bool HandleInput(float frameTime);
	bool RenderGraphics();

};

