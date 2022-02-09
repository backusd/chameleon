#pragma once
#include "pch.h"
#include "ChameleonException.h"

#include <string>
#include <sstream>

class TextureException : public ChameleonException
{
public:
	TextureException(int line, const char* file, std::string description) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	std::string GetErrorInfo() const noexcept;
private:
	std::string m_info;
};