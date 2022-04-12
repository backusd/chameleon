#pragma once
#include "pch.h"
#include "ChameleonException.h"

#include <string>
#include <sstream>

class ModelException : public ChameleonException
{
public:
	ModelException(int line, const char* file, std::string description) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;
private:
	std::string m_info;
};