#pragma once
#include "pch.h"
#include "ChameleonException.h"

#include <sstream>

class WindowException : public ChameleonException
{
public:
	WindowException(int line, const char* file, HRESULT hr) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	static std::string TranslateErrorCode(HRESULT hr) noexcept;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorString() const noexcept;
private:
	HRESULT hr;
};

// error exception helper macro
#define WINDOW_EXCEPT( hr ) WindowException( __LINE__,__FILE__,hr )
#define WINDOW_LAST_EXCEPT() WindowException( __LINE__,__FILE__,GetLastError() )