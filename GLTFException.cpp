#include "GLTFException.h"

GLTFException::GLTFException(int line, const char* file, std::string description) noexcept :
	ChameleonException(line, file)
{
	m_info = description;
}

const char* GLTFException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* GLTFException::GetType() const noexcept
{
	return "GLTF Exception";
}

std::string GLTFException::GetErrorInfo() const noexcept
{
	return m_info;
}