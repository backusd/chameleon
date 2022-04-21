#include "Base64Exception.h"

Base64Exception::Base64Exception(int line, const char* file, std::string description) noexcept :
	ChameleonException(line, file)
{
	m_info = description;
}

const char* Base64Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* Base64Exception::GetType() const noexcept
{
	return "Base64 Exception";
}

std::string Base64Exception::GetErrorInfo() const noexcept
{
	return m_info;
}