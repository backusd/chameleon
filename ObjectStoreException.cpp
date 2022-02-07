#include "ObjectStoreException.h"

ObjectStoreException::ObjectStoreException(int line, const char* file, const char* mapVariable, std::string mapKey) noexcept :
	ChameleonException(line, file)
{
	std::ostringstream oss;
	oss << "The key '" << mapKey << "' does not exist in the map '" << mapVariable << "'";
	m_info = oss.str();
}


const char* ObjectStoreException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* ObjectStoreException::GetType() const noexcept
{
	return "Object Store Exception";
}

std::string ObjectStoreException::GetErrorInfo() const noexcept
{
	return m_info;
}