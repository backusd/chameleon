#pragma once
#include "pch.h"

#include "WindowBaseTemplate.h"

#include <sstream>

class WindowBase : public WindowBaseTemplate<WindowBase>
{
public:
	WindowBase(int width, int height, const char* name);
	virtual ~WindowBase() {}

	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;

	bool NeedsDestroy() { return m_destroy; }
	virtual void Destroy() {}

	virtual void Update() {}
	virtual bool Render() { return true; }
	virtual void Present() {}



protected:

	// Destroy flag
	bool m_destroy;

	// Virtual functions to handle all input 
	virtual LRESULT OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnClose(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	virtual LRESULT OnLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnLButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnMButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnMButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnRButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnRButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	virtual LRESULT OnPaint(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnResize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	virtual LRESULT OnMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnMouseLeave(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnMouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	virtual LRESULT OnGetMinMaxInfo(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	virtual LRESULT OnChar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnSysKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	virtual LRESULT OnSysKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	virtual LRESULT OnKillFocus(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
};