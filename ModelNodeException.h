#pragma once
#include "pch.h"
#include "ChameleonException.h"

#include <string>
#include <sstream>

class ModelNodeException : public ChameleonException
{
public:
	ModelNodeException(int line, const char* file, std::string description) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;
private:
	std::string m_info;
};