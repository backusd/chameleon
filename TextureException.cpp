#include "TextureException.h"

TextureException::TextureException(int line, const char* file, std::string description) noexcept :
	ChameleonException(line, file)
{
	m_info = description;
}


const char* TextureException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* TextureException::GetType() const noexcept
{
	return "Texture Exception";
}

std::string TextureException::GetErrorInfo() const noexcept
{
	return m_info;
}