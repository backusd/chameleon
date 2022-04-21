#include "ModelNodeException.h"

ModelNodeException::ModelNodeException(int line, const char* file, std::string description) noexcept :
	ChameleonException(line, file)
{
	m_info = description;
}

const char* ModelNodeException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* ModelNodeException::GetType() const noexcept
{
	return "Model Node Exception";
}

std::string ModelNodeException::GetErrorInfo() const noexcept
{
	return m_info;
}