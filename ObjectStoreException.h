#pragma once
#include "pch.h"
#include "ChameleonException.h"

#include <string>
#include <sstream>

class ObjectStoreException : public ChameleonException
{
public:
	ObjectStoreException(int line, const char* file, const char* mapVariable, std::string mapKey) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;
private:
	std::string m_info;
};