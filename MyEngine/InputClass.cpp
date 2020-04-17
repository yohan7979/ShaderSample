#include "InputClass.h"

InputClass::InputClass()	:
	m_directInput(0),
	m_keyboard(0),
	m_mouse(0)
{
}

InputClass::InputClass(const InputClass &other)
{
}


InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_mouseX = 0;
	m_mouseY = 0;

	// DirectX 입력 인터페이스 초기화
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드 인터페이스 초기화
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 데이터 형식 설정(디폴트)
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// EXCLUSIVE로 설정해 현재 실행한 프로그램만 입력을 받도록 한다.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 키보드 인터페이스 세팅
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// 마우스 인터페이스 초기화
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 데이터 형식 설정(디폴트)
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// 마우스 인터페이스 세팅
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}
}

bool InputClass::Frame()
{
	bool result;


	// 키보드 입력을 받는다.
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// 마우스 입력을 받는다.
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// 받은 입력을 처리한다.
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;


	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;


	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}

bool InputClass::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsLeftPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if (m_keyboardState[DIK_LEFTARROW] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsRightPressed()
{
	if (m_keyboardState[DIK_RIGHTARROW] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsUpPressed()
{
	if (m_keyboardState[DIK_UPARROW] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsDownPressed()
{
	if (m_keyboardState[DIK_DOWNARROW] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsWPressed()
{
	if (m_keyboardState[DIK_W] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsSPressed()
{
	if (m_keyboardState[DIK_S] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsAPressed()
{
	if (m_keyboardState[DIK_A] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsDPressed()
{
	if (m_keyboardState[DIK_D] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsF1Pressed()
{
	if (m_keyboardState[DIK_F1] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsF2Pressed()
{
	if (m_keyboardState[DIK_F2] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsF3Pressed()
{
	if (m_keyboardState[DIK_F3] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::Is1Pressed()
{
	if (m_keyboardState[DIK_1] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::Is2Pressed()
{
	if (m_keyboardState[DIK_2] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::Is3Pressed()
{
	if (m_keyboardState[DIK_3] & 0x80)
	{
		return true;
	}
	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
}
