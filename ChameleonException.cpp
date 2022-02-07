#include "ChameleonException.h"
#include <sstream>


ChameleonException::ChameleonException(int line, const char* file) noexcept :
	m_line(line),
	m_file(file)
{
}

const char* ChameleonException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* ChameleonException::GetType() const noexcept
{
	return "Chameleon Exception";
}

int ChameleonException::GetLine() const noexcept
{
	return m_line;
}

const std::string& ChameleonException::GetFile() const noexcept
{
	return m_file;
}

std::string ChameleonException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << m_file << std::endl
		<< "[Line] " << m_line;
	return oss.str();
}
