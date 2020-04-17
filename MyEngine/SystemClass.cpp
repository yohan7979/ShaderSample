#include "SystemClass.h"
#include "inputclass.h"
#include "graphicsclass.h"
#include "FpsClass.h"
#include "CpuClass.h"
#include "TimerClass.h"
#include "ApplicationClass.h"


SystemClass::SystemClass()	:
	m_Input(0), m_Graphics(0),
	m_Fps(0), m_Cpu(0),
	m_Timer(0)
{
}

SystemClass::SystemClass(const SystemClass & other)
{
}


SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	screenWidth = 0;
	screenHeight = 0;

	// 윈도우 api를 초기화한다.
	InitializeWindows(screenWidth, screenHeight);

	// 입력 관리자를 생성한다.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// 그래픽 관리자를 초기화한다.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	// Fps 클래스 초기화
	m_Fps = new FpsClass;
	if (!m_Fps)
	{
		return false;
	}
	m_Fps->Initialize();

	// Cpu 클래스 초기화
	m_Cpu = new CpuClass;
	if (!m_Cpu)
	{
		return false;
	}
	m_Cpu->Initialize();

	// 타이머 클래스 초기화
	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		return false;
	}
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{

	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;

	}
	// 그래픽 관리자의 메모리를 해제한다.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// 입력 관리자의 메모리를 해제한다.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// 윈도우를 종료한다.
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));
	done = false;

	// 종료 메세지를 받기 전까지 루프를 돈다.
	while (!done)
	{
		// 윈도우 메세지 핸들
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 종료 메세지를 받는다면
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// 강제 종료
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
		// Check if the user pressed escape and wants to quit.
		if (m_Input->IsEscapePressed() == true)
		{
			done = true;
		}

	}

	return;
}

// DirectX Input으로 입력을 처리하므로 메시지 핸들러 함수는 비워준다.
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

bool SystemClass::Frame()
{
	bool result;
	
	// Fps, Cpu, Timer를 처리한다.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// 사용자 입력을 처리한다.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// 마우스 좌표를 얻어온다.
	int mouseX, mouseY;
	m_Input->GetMouseLocation(mouseX, mouseY);


	// 그래픽 클래스의 프레임 단위 처리를 수행한다.
	result = m_Graphics->Frame(mouseX, mouseY, 
		m_Fps->GetFPS(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime(), m_Input);
	if (!result)
	{
		return false;
	}



	return true;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 윈도우 제거
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
