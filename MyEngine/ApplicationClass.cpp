#include "ApplicationClass.h"



ApplicationClass::ApplicationClass()
{

}

ApplicationClass::ApplicationClass(const ApplicationClass & other)
{
}


ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	return true;
}

void ApplicationClass::Shutdown()
{
}

bool ApplicationClass::Frame()
{
	return true;
}

bool ApplicationClass::HandleInput(float frameTime)
{
	return true;
}

bool ApplicationClass::RenderGraphics()
{
	return true;
}
