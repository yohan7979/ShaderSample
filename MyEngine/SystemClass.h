#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

// Win32 ����� ũ�⸦ �ٿ��ش�. (���ʿ��� api�� ����)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// ��ü �ý����� ����, �����ϴ� Ŭ����
// ������ �޼����� �޾� �ۿ� �Ѱ��ְ�, �Է� ó���� �������� �����Ѵ�.
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