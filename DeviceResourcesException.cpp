#include "DeviceResourcesException.h"

// DeviceResourcesException ===========================================================================

DeviceResourcesException::DeviceResourcesException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	ChameleonException(line, file),
	hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		m_info += m;
		m_info.push_back('\n');
	}
	// remove final newline if exists
	if (!m_info.empty())
	{
		m_info.pop_back();
	}
}

const char* DeviceResourcesException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << TranslateErrorCode(hr) << std::endl;

	if (!m_info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* DeviceResourcesException::GetType() const noexcept
{
	return "Device Resources Exception";
}

HRESULT DeviceResourcesException::GetErrorCode() const noexcept
{
	return hr;
}

std::string DeviceResourcesException::GetErrorInfo() const noexcept
{
	return m_info;
}

std::string DeviceResourcesException::TranslateErrorCode(HRESULT hr) noexcept
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


// InfoException ====================================================================================
InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept :
	ChameleonException(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		m_info += m;
		m_info.push_back('\n');
	}
	// remove final newline if exists
	if (!m_info.empty())
	{
		m_info.pop_back();
	}
}


const char* InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* InfoException::GetType() const noexcept
{
	return "Graphics Info Exception";
}

std::string InfoException::GetErrorInfo() const noexcept
{
	return m_info;
}



// DeviceRemovedException ===========================================================================
DeviceRemovedException::DeviceRemovedException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept :
	DeviceResourcesException(line, file, hr, infoMsgs)
{}


const char* DeviceRemovedException::GetType() const noexcept
{
	return "Device Resources Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}