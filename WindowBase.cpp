#include "WindowBase.h"


#include "WindowsMessageMap.h"


WindowBase::WindowBase(int width, int height, const char* name) :
	WindowBaseTemplate(width, height, name),
	m_destroy(false)
{
}

LRESULT WindowBase::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//static WindowsMessageMap mm;
	//OutputDebugString(mm(msg, wParam, lParam).c_str());	

	switch (msg)
	{
	case WM_CREATE:			return OnCreate(hWnd, msg, wParam, lParam);
	case WM_CLOSE:			return OnClose(hWnd, msg, wParam, lParam);
	case WM_DESTROY:		return OnDestroy(hWnd, msg, wParam, lParam);
	case WM_LBUTTONDOWN:	return OnLButtonDown(hWnd, msg, wParam, lParam);
	case WM_LBUTTONUP:		return OnLButtonUp(hWnd, msg, wParam, lParam);
	case WM_LBUTTONDBLCLK:	return OnLButtonDoubleClick(hWnd, msg, wParam, lParam);
	case WM_MBUTTONDOWN:	return OnMButtonDown(hWnd, msg, wParam, lParam);
	case WM_MBUTTONUP:		return OnMButtonUp(hWnd, msg, wParam, lParam);
	case WM_RBUTTONDOWN:	return OnRButtonDown(hWnd, msg, wParam, lParam);
	case WM_RBUTTONUP:		return OnRButtonUp(hWnd, msg, wParam, lParam);
	case WM_PAINT:			return OnPaint(hWnd, msg, wParam, lParam);
	case WM_SIZE:			return OnResize(hWnd, msg, wParam, lParam);
	case WM_MOUSEMOVE:		return OnMouseMove(hWnd, msg, wParam, lParam);
	case WM_MOUSELEAVE:		return OnMouseLeave(hWnd, msg, wParam, lParam);
	case WM_MOUSEWHEEL:		return OnMouseWheel(hWnd, msg, wParam, lParam);
	case WM_GETMINMAXINFO:	return OnGetMinMaxInfo(hWnd, msg, wParam, lParam);
	case WM_CHAR:			return OnChar(hWnd, msg, wParam, lParam);
	case WM_KEYUP:			return OnKeyUp(hWnd, msg, wParam, lParam);
	case WM_KEYDOWN:		return OnKeyDown(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowBase::OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowBase::OnClose(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// we don't want the DefProc to handle this message because
	// we want our destructor to destroy the window, so return 0 instead of break

	// Don't post quit - Just set the destroy window flag
	// WindowManager will worry about posting WM_QUIT
	m_destroy = true;

	/* Perform any clean up work here */

	return 0;
}

LRESULT WindowBase::OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowBase::OnLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowBase::OnLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowBase::OnLButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowBase::OnMButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowBase::OnMButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowBase::OnRButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowBase::OnRButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowBase::OnPaint(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowBase::OnResize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowBase::OnMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowBase::OnMouseLeave(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WindowBase::OnMouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowBase::OnGetMinMaxInfo(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowBase::OnChar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowBase::OnKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WindowBase::OnKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

