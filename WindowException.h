#pragma once
#include "pch.h"
#include "ChameleonException.h"

#include <sstream>


class WindowExceptionBase : public ChameleonException
{
public:
	WindowExceptionBase(int line, const char* file) noexcept;
	static std::string TranslateErrorCode(HRESULT hr) noexcept;
};

class WindowException : public WindowExceptionBase
{
public:
	WindowException(int line, const char* file, HRESULT hr) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorDescription() const noexcept;
private:
	HRESULT hr;
};

class NoGfxException : public WindowExceptionBase
{
public:
	const char* GetType() const noexcept override;
};

// error exception helper macro
#define WINDOW_EXCEPT( hr ) WindowException( __LINE__,__FILE__,hr )
#define WINDOW_LAST_EXCEPT() WindowException( __LINE__,__FILE__,GetLastError() )
#define WINDOW_NOGFX_EXCEPT() NoGfxException(__LINE__, __FILE__)