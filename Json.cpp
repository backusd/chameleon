#include "Json.h"

Json::Json()
{

}

std::any& Json::operator[](std::string key)
{
	// If the key does not currently exist in the map, create an empty entry (so the key can be assigned to)
	if (m_map.find(key) == m_map.end()) 
		m_map.insert(std::pair(key, std::any()));

	return m_map[key];
}

// ======================================================================================================

Json JsonReader::ReadFile(std::string filename)
{
	// Open the file -------------------------------------------------
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::ostringstream oss;
		oss << "Could not open file: " << filename;
		throw JsonException(__LINE__, __FILE__, oss.str());
	}

	// Loop over each line in the file and join them into a single string -------------------------------
	std::string line;
	std::string all;
	while (std::getline(file, line))
	{
		all += line;
	}

	file.close();

	// Getting the next Json object should parse the entire json file
	unsigned int iii = 0u;
	return JsonReader::GetNextJson(all, iii);
}

void JsonReader::MoveToNextNonSpace(std::string& fileText, unsigned int& index)
{
	// Iterate the index until you find a non-space character
	for (; index < fileText.size(); ++index)
		if (fileText[index] != ' ')
			break;
}

Json JsonReader::GetNextJson(std::string& fileText, unsigned int& index)
{
	JsonReader::MoveToNextNonSpace(fileText, index);

	int startIndex = index;

	// first character for the next json object should always be '{'
	std::ostringstream oss;
	if (fileText[index] != '{')
	{		
		oss << "No Json object starting at index: " << index;
		throw JsonException(__LINE__, __FILE__, oss.str());
	}

	// Initialize a new Json object
	Json json;

	// bool for whether we are getting a key or getting a value
	bool gettingKey = true;

	std::string key;
	std::string number;

	++index;
	for (; index < fileText.size(); ++index)
	{
		// If you find an end bracket, the Json object is complete
		switch (fileText[index])
		{
		case '}':
			// If we are trying to get a value for a key, then throw an error
			if (!gettingKey)
			{
				oss << "No value for key '" << key << "'";
				throw JsonException(__LINE__, __FILE__, oss.str());
			}
			return json;	// NOTE: when a json object is returned, the index will be the index of the closing end bracket
		
		case ' ': continue;
		
		case '\"': 
			// If we are looking for a key, get the next string value as the key
			// NOTE: When this returns, the index will be the index of the closing
			//		 quote. This is actually good, because the for-loop will increment
			//		 to the next character in the next iteration
			if (gettingKey)
			{
				key = JsonReader::GetNextString(fileText, index);
				gettingKey = false;
			}
			else
			{
				// We already have the key, so get the next string as the value for the key
				json[key] = JsonReader::GetNextString(fileText, index);
				gettingKey = true;
			}
			continue;
		
		case ':':
			if (gettingKey)
			{
				oss << "Found ':' at index " << index << " but was looking for a key";
				throw JsonException(__LINE__, __FILE__, oss.str());
			}
			continue;

		case '{':
			if (gettingKey)
			{
				oss << "Found '{' at index " << index << " but was looking for a key";
				throw JsonException(__LINE__, __FILE__, oss.str());
			}

			// Get the next Json object as the value for the current key
			// NOTE: This will increment the index to the closing bracket for the Json object
			//		 This is good because the for-loop will increment the index in the next
			//		 loop iteration
			json[key] = JsonReader::GetNextJson(fileText, index);
			gettingKey = true;
			continue;

		case '.':
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (gettingKey)
			{
				oss << "Found the number " << fileText[index] << " at index " << index << " but was looking for a key";
				throw JsonException(__LINE__, __FILE__, oss.str());
			}

			// Get the number as a string. If it contains a decimal, then cast to a double, otherwise int
			number = JsonReader::GetNextNumber(fileText, index);
			json[key] = number.contains(".") ? std::stod(number) : std::stoi(number);
			gettingKey = true;
			continue;

		case 't':
		case 'f':
			if (gettingKey)
			{
				oss << "Found the letter '" << fileText[index] << "' at index " << index << " but was looking for a key";
				throw JsonException(__LINE__, __FILE__, oss.str());
			}
			json[key] = JsonReader::GetNextBoolean(fileText, index);
			gettingKey = true;
			continue;

		case '[':
			if (gettingKey)
			{
				oss << "Found '[' at index " << index << " but was looking for a key";
				throw JsonException(__LINE__, __FILE__, oss.str());
			}

			// Get the next array object as the value for the current key
			// NOTE: This will increment the index to the closing bracket for the array object
			//		 This is good because the for-loop will increment the index in the next
			//		 loop iteration
			json[key] = JsonReader::GetNextVector(fileText, index);
			gettingKey = true;
			continue;

		case ',':
			// In the case of a comma, make sure that we have finished getting the previous value and
			// are again searching for the next key
			if (!gettingKey)
			{
				oss << "Found ',' at index " << index << " but was looking for a value";
				throw JsonException(__LINE__, __FILE__, oss.str());
			}
			continue;

		default:
			oss << "Invalid character '" << fileText[index] << "' while parsing Json object at index " << index;
			throw JsonException(__LINE__, __FILE__, oss.str());
		}
	}

	oss << "Did not find closing bracket for Json object starting at index: " << startIndex;
	throw JsonException(__LINE__, __FILE__, oss.str());
}

std::string JsonReader::GetNextString(std::string& fileText, unsigned int& index)
{
	JsonReader::MoveToNextNonSpace(fileText, index);

	int startIndex = index;

	// first character for the next string should always be '\"'
	if (fileText[index] != '\"')
	{
		std::ostringstream oss;
		oss << "No string object starting at index: " << index;
		throw JsonException(__LINE__, __FILE__, oss.str());
	}

	// Iterate until you find the end of the quote
	std::string s;
	++index;
	for (; index < fileText.size(); ++index)
	{
		if (fileText[index] == '\"')
			return s;		// NOTE: the index when returning is the index for the closing quote

		s += fileText[index];
	}

	std::ostringstream oss;
	oss << "Did not find closing quote for string starting at index: " << startIndex;
	throw JsonException(__LINE__, __FILE__, oss.str());
}

std::string JsonReader::GetNextNumber(std::string& fileText, unsigned int& index)
{
	JsonReader::MoveToNextNonSpace(fileText, index);

	int startIndex = index;

	// Iterate until you find the end of the quote
	std::string validChars = ".-0123456789";
	std::string s;
	for (; index < fileText.size(); ++index)
	{
		// If the next character is not a valid number character, decrement the index to be
		// the value of the last valid char and return the string
		if (!validChars.contains(fileText[index]))
		{	
			--index;
			return s;
		}

		s += fileText[index];
	}

	std::ostringstream oss;
	oss << "Did not find an invalid numerical character for the number starting at index: " << startIndex;
	throw JsonException(__LINE__, __FILE__, oss.str());
}

std::vector<std::any> JsonReader::GetNextVector(std::string& fileText, unsigned int& index)
{
	JsonReader::MoveToNextNonSpace(fileText, index);

	int startIndex = index;
	std::ostringstream oss;

	// first character for the array should always be '['
	if (fileText[index] != '[')
	{
		oss << "No vector object starting at index: " << index;
		throw JsonException(__LINE__, __FILE__, oss.str());
	}

	std::vector<std::any> v;
	std::string number;

	++index;
	for (; index < fileText.size(); ++index)
	{
		// If you find an end bracket, the vector object is complete
		switch (fileText[index])
		{
		case ']':
			return v;	// NOTE: when a vector object is returned, the index will be the index of the closing end bracket

		case ' ': continue;

		case '\"':
			// Get the next string object
			v.push_back(JsonReader::GetNextString(fileText, index));
			continue;

		case '{':
			// Get the next Json object
			v.push_back(JsonReader::GetNextJson(fileText, index));
			continue;

		case '.':
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			// Get the number as a string. If it contains a decimal, then cast to a double, otherwise int
			number = JsonReader::GetNextNumber(fileText, index);
			if (number.contains("."))
				v.push_back(std::stod(number));
			else
				v.push_back(std::stoi(number));
			continue;

		case 't':
		case 'f':
			// Get the next boolean value
			v.push_back(JsonReader::GetNextBoolean(fileText, index));
			continue;

		case '[':
			// Get the next Json object
			v.push_back(JsonReader::GetNextVector(fileText, index));
			continue;

		// In the case of a comma, just continue to get the next value
		case ',': continue;

		default:
			oss << "Invalid character '" << fileText[index] << "' while parsing vector object at index " << index;
			throw JsonException(__LINE__, __FILE__, oss.str());
		}
	}

	oss << "Did not find closing square bracket for vector starting at index: " << startIndex;
	throw JsonException(__LINE__, __FILE__, oss.str());
}

bool JsonReader::GetNextBoolean(std::string& fileText, unsigned int& index)
{
	std::ostringstream oss;

	if (fileText[index] == 't')
	{
		if (fileText.substr(index, 4) == "true")
		{
			index += 3;
			return true;
		}
		oss << "Unrecognized substring '" << fileText.substr(index, 4) << "' at index " << index << " when looking for a boolean";
		throw JsonException(__LINE__, __FILE__, oss.str());
	}
	else if (fileText[index] == 'f')
	{
		if (fileText.substr(index, 5) == "false")
		{
			index += 4;
			return false;
		}
		oss << "Unrecognized substring '" << fileText.substr(index, 5) << "' at index " << index << " when looking for a boolean";
		throw JsonException(__LINE__, __FILE__, oss.str());
	}
	else
	{
		oss << "Invalid starting character '" << fileText[index] << "' at index " << index << " when looking for a boolean";
		throw JsonException(__LINE__, __FILE__, oss.str());
	}
}