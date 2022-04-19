#pragma once
#include "pch.h"
#include "Base64Exception.h"

#include <string>
#include <vector>
#include <stdint.h>
#include <sstream>

class Base64
{
public:
	static std::vector<uint8_t> Decode(const std::string& data);
	static std::string Encode(const std::vector<uint8_t>& in);

private:
	Base64() {} // Disallow creation of an Base64 object

	static size_t Decode(const std::string& in, std::vector<uint8_t>& out);
	static size_t Decode(const char* in, size_t inLength, uint8_t*& out);

	static void Encode(const std::vector<uint8_t>& in, std::string& out);
	static void Encode(const uint8_t* in, size_t inLength, std::string& out);

	static inline uint8_t DecodeChar(char c);
	static inline char EncodeChar(uint8_t b);

	static const uint8_t tableDecodeBase64[128];
	static const char* tableEncodeBase64;
};