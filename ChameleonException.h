#pragma once
#include "pch.h"

#include <exception>
#include <string>

class ChameleonException : public std::exception
{
public:
	ChameleonException(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
private:
	int m_line;
	std::string m_file;
protected:
	mutable std::string m_whatBuffer;
};