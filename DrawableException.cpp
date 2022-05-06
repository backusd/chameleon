#include "DrawableException.h"

DrawableException::DrawableException(int line, const char* file, std::string description) noexcept :
	ChameleonException(line, file)
{
	m_info = description;
}

const char* DrawableException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* DrawableException::GetType() const noexcept
{
	return "Drawable Exception";
}

std::string DrawableException::GetErrorInfo() const noexcept
{
	return m_info;
}