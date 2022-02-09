#include "TerrainMeshException.h"

TerrainMeshException::TerrainMeshException(int line, const char* file, std::string description) noexcept :
	ChameleonException(line, file)
{
	m_info = description;
}


const char* TerrainMeshException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

const char* TerrainMeshException::GetType() const noexcept
{
	return "Terrain Mesh Exception";
}

std::string TerrainMeshException::GetErrorInfo() const noexcept
{
	return m_info;
}