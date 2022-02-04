#include "ChameleonException.h"
#include <sstream>


ChameleonException::ChameleonException(int line, const char* file) noexcept :
	line(line),
	file(file)
{
}

const char* ChameleonException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ChameleonException::GetType() const noexcept
{
	return "Chameleon Exception";
}

int ChameleonException::GetLine() const noexcept
{
	return line;
}

const std::string& ChameleonException::GetFile() const noexcept
{
	return file;
}

std::string ChameleonException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
