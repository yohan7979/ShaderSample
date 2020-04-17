#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

// Win32 헤더의 크기를 줄여준다. (불필요한 api를 제외)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// 전체 시스템을 관리, 구동하는 클래스
// 윈도우 메세지를 받아 앱에 넘겨주고, 입력 처리와 렌더링을 수행한다.
class InputClass;
class GraphicsClass;
class FpsClass;
class CpuClass;
class TimerClass;

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	GraphicsClass* m_Graphics;
	FpsClass* m_Fps;
	CpuClass* m_Cpu;
	TimerClass* m_Timer;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;

#endif