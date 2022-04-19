#pragma once
#include "pch.h"
#include "JsonException.h"

#include <string>
#include <map>
#include <any>
#include <memory>
#include <fstream>
#include <vector>

class Json
{
public:
	Json();
	std::any& operator[](std::string key);

	template <typename T>
	T Get(std::string key);

private:
	std::map<std::string, std::any> m_map;
};

template <typename T>
T Json::Get(std::string key)
{
	if (m_map.find(key) == m_map.end())
	{
		std::ostringstream oss;
		oss << "Json::Get -> Could not find key: " << key;
		throw JsonException(__LINE__, __FILE__, oss.str());
	}

	return std::any_cast<T>(m_map[key]);
}


// =======================================================================

class JsonReader
{
public:
	static Json ReadFile(std::string filename);

private:
	JsonReader() {} // Disallow creation of an JsonReader object

	static void MoveToNextNonSpace(std::string& fileText, unsigned int& index);

	static Json GetNextJson(std::string& fileText, unsigned int& index);
	static std::string GetNextString(std::string& fileText, unsigned int& index);
	static std::string GetNextNumber(std::string& fileText, unsigned int& index);
	static std::vector<std::any> GetNextVector(std::string& fileText, unsigned int& index);
	static bool GetNextBoolean(std::string& fileText, unsigned int& index);

};