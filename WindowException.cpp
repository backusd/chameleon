#include "WindowException.h"

// WindowExceptionBase ===================================================================================
WindowExceptionBase::WindowExceptionBase(int line, const char* file) noexcept :
	ChameleonException(line, file)
{
}

std::string WindowExceptionBase::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}


// WindowException =========================================================================================
WindowException::WindowException(int line, const char* file, HRESULT hr) noexcept :
	WindowExceptionBase(line, file),
	hr(hr)
{
}

const char* WindowException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* WindowException::GetType() const noexcept
{
	return "Window Exception";
}

HRESULT WindowException::GetErrorCode() const noexcept
{
	return hr;
}

std::string WindowException::GetErrorDescription() const noexcept
{
	return TranslateErrorCode(hr);
}

// NoGfxException ==========================================================================================
const char* NoGfxException::GetType() const noexcept
{
	return "Window Exception [No Graphics]";
}