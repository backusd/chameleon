#include "ContentWindow.h"

using Microsoft::WRL::ComPtr;
using DirectX::XMFLOAT3;


/////////////
// GLOBALS //
/////////////
//const bool FULL_SCREEN = true;
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


ContentWindow::ContentWindow(int width, int height, const char* name) :
	WindowBase(width, height, name)
{
	bool result;

	/*
	
	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
	if (!m_Input)
	{
		throw new ChameleonException(0, nullptr);
	}

	// Initialize the input object.
	result = m_Input->Initialize(m_hInst, m_hWnd, width, height);
	if (!result)
	{
		MessageBox(m_hWnd, "Could not initialize the input object.", "Error", MB_OK);
		throw new ChameleonException(0, nullptr);
	}

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		throw new ChameleonException(0, nullptr);
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(width, height, VSYNC_ENABLED, m_hWnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(m_hWnd, "Could not initialize Direct3D.", "Error", MB_OK);
		throw new ChameleonException(0, nullptr);
	}

	// Create the timer object.
	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		throw new ChameleonException(0, nullptr);
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(m_hWnd, "Could not initialize the timer object.", "Error", MB_OK);
		throw new ChameleonException(0, nullptr);
	}

	// Create the fps object.
	m_Fps = new FpsClass;
	if (!m_Fps)
	{
		throw new ChameleonException(0, nullptr);
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the cpu object.
	m_Cpu = new CpuClass;
	if (!m_Cpu)
	{
		throw new ChameleonException(0, nullptr);
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create the user interface object.
	m_UserInterface = new UserInterfaceClass;
	if (!m_UserInterface)
	{
		throw new ChameleonException(0, nullptr);
	}

	// Initialize the user interface object.
	result = m_UserInterface->Initialize(m_D3D, m_hWnd, width, height);
	if (!result)
	{
		MessageBox(m_hWnd, "Could not initialize the user interface object.", "Error", MB_OK);
		throw new ChameleonException(0, nullptr);
	}

	// Create the StateClass object.
	m_State = new StateClass;
	if (!m_State)
	{
		throw new ChameleonException(0, nullptr);
	}

	// Create the black forest object.
	m_BlackForest = new BlackForestClass;
	if (!m_BlackForest)
	{
		throw new ChameleonException(0, nullptr);
	}

	// Initialize the black forest object.
	result = m_BlackForest->Initialize(m_D3D, m_hWnd, width, height, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(m_hWnd, "Could not initialize the black forest object.", "Error", MB_OK);
		throw new ChameleonException(0, nullptr);
	}

	// Create the network object.
	m_Network = new NetworkClass;
	if (!m_Network)
	{
		throw new ChameleonException(0, nullptr);
	}

	// Set the zone and UI pointer before initializing.
	m_Network->SetZonePointer(m_BlackForest);
	m_Network->SetUIPointer(m_UserInterface);

	// Initialize the network object.
	char ip[] = "155.248.215.180";
	result = m_Network->Initialize(ip, 7000);
	if (!result)
	{
		MessageBox(m_hWnd, "Could not initialize the network.", "Error", MB_OK);
		throw new ChameleonException(0, nullptr);
	}

	*/
}

ContentWindow::~ContentWindow()
{

}

void ContentWindow::Update()
{

}

bool ContentWindow::Render()
{
	/*
	bool result;
	char state;
	float posX, posY, posZ, rotX, rotY, rotZ;


	// Update the system stats.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// Do the input frame processing.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Do the network frame processing.
	m_Network->Frame();

	// Do the UI frame processing.
	result = m_UserInterface->Frame(m_D3D, m_Input, m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), m_Network->GetLatency());
	if (!result)
	{
		return false;
	}

	// Do the zone frame processing based on the current zone state.
	switch (m_State->GetCurrentState())
	{
	case STATE_BLACKFOREST:
	{
		result = m_BlackForest->Frame(m_D3D, m_Input, m_Timer->GetTime(), m_UserInterface);

		// Check for state changes.
		if (m_BlackForest->GetStateChange(state) == true)
		{
			m_Network->SendStateChange(state);
		}

		// Check for position updates.
		if (m_BlackForest->PositionUpdate(posX, posY, posZ, rotX, rotY, rotZ) == true)
		{
			m_Network->SendPositionUpdate(posX, posY, posZ, rotX, rotY, rotZ);
		}

		break;
	}
	default:
	{
		result = true;
		break;
	}
	}

	return result;

	*/

	return true;
}

void ContentWindow::Present()
{

}

void ContentWindow::Destroy()
{
	/*
	// Release the black forest object.
	if (m_BlackForest)
	{
		m_BlackForest->Shutdown();
		delete m_BlackForest;
		m_BlackForest = 0;
	}

	// Release the StateClass object.
	if (m_State)
	{
		delete m_State;
		m_State = 0;
	}

	// Release the user interface object.
	if (m_UserInterface)
	{
		m_UserInterface->Shutdown();
		delete m_UserInterface;
		m_UserInterface = 0;
	}

	// Release the cpu object.
	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the network object.
	if (m_Network)
	{
		m_Network->Shutdown();
		delete m_Network;
		m_Network = 0;
	}

	// Release the D3D object.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	*/
}






LRESULT ContentWindow::OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return 0;
}

LRESULT ContentWindow::OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	DiscardGraphicsResources();
	return 0;
}

LRESULT ContentWindow::OnLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnLButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnRButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnRButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT ContentWindow::OnResize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return 0;
}

LRESULT ContentWindow::OnMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMouseLeave(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ContentWindow::DiscardGraphicsResources()
{

}

float ContentWindow::Height()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return static_cast<float>(rect.bottom);
}
float ContentWindow::Width()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return static_cast<float>(rect.right);
}

LRESULT ContentWindow::OnGetMinMaxInfo(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT ContentWindow::OnChar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

