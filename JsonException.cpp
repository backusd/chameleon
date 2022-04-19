#include "JsonException.h"

JsonException::JsonException(int line, const char* file, std::string description) noexcept :
	ChameleonException(line, file)
{
	m_info = description;
}

const char* JsonException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* JsonException::GetType() const noexcept
{
	return "Json Exception";
}

std::string JsonException::GetErrorInfo() const noexcept
{
	return m_info;
}