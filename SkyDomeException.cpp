#include "SkyDomeException.h"

SkyDomeException::SkyDomeException(int line, const char* file, std::string description) noexcept :
	ChameleonException(line, file)
{
	m_info = description;
}


const char* SkyDomeException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* SkyDomeException::GetType() const noexcept
{
	return "Sky Dome Exception";
}

std::string SkyDomeException::GetErrorInfo() const noexcept
{
	return m_info;
}