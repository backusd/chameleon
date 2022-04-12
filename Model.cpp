#include "Model.h"

using DirectX::XMMATRIX;

Model::Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::shared_ptr<Mesh> mesh) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController)
{
	// Create the root node - will initially have no data and no name
	m_rootNode = std::make_unique<ModelNode>(m_deviceResources, m_moveLookController);
	m_rootNode->SetMesh(mesh);
}

Model::Model(std::shared_ptr<DeviceResources> deviceResources, std::shared_ptr<MoveLookController> moveLookController, std::string fileName) :
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController),
	m_rootNode(nullptr)
{
	Load(fileName);
}

void Model::Load(std::string filename)
{
	const std::filesystem::path filePath = filename;
	std::string extension = filePath.extension().string();
	if (extension == ".obj")
	{
		OBJLoadFile(filename);
	}
	else
	{
		std::ostringstream oss;
		oss << "Unsupported 3D model format: " << filename;
		throw ModelException(__LINE__, __FILE__, oss.str());
	}
}

void Model::OBJLoadFile(std::string filename)
{
	// OBJ files do not have a hierarchical structure, but they can contain multiple meshes
	// Therefore, parse the first one as the root node and all subsequent ones will be added
	// as children to the root node


	// Parse all positions, texture coordinates, and normals first before creating the final vertices
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT2> textures;
	std::vector<XMFLOAT3> normals;

	OBJGetPositionsTexturesNormals(filename, positions, textures, normals);

	// Throw exception if we didn't get any positions
	if (positions.size() == 0)
	{
		std::ostringstream oss;
		oss << "Did not read any position data from OBJ file: " << filename;
		throw ModelException(__LINE__, __FILE__, oss.str());
	}

	// Loop over the face values in the file and create the ModelNodes
	OBJCreateVertices(filename, positions, textures, normals);
}

void Model::OBJGetPositionsTexturesNormals(std::string filename, std::vector<XMFLOAT3>& positions, std::vector<XMFLOAT2>& textures, std::vector<XMFLOAT3>& normals)
{
	// Open the file -------------------------------------------------
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::ostringstream oss;
		oss << "Could not open file: " << filename;
		throw ModelException(__LINE__, __FILE__, oss.str());
	}

	// Parsing variables ---------------------------------------------
	int lineNumber = 0;						// Used in exception messages for debugging
	std::vector<std::string> lineTokens;	// vector to hold each string parsed from a line
	std::istringstream lineISS;				// istringstream for parsing line tokens
	std::string lineItem;					// temporary string to hold parsed token to be added to token vector

	// Loop over each line in the file -------------------------------
	std::string line;
	while (std::getline(file, line))
	{
		// Update the line number
		++lineNumber;

		// Get each string in the line, splitting on space character ' '
		lineTokens.clear();
		lineISS = std::istringstream(line);
		while (std::getline(lineISS, lineItem, ' '))
			lineTokens.push_back(lineItem);

		if (lineTokens.size() == 0)
			continue;

		// If token is 'v', the next 3 values in the line are for a vertex position
		if (lineTokens[0] == "v")
		{
			// Make sure the number of tokens in the line is exactly 4
			if (lineTokens.size() != 4)
			{
				std::ostringstream oss;
				oss << "File '" << filename << "' has invalid number of vertex parameters at line " << lineNumber;
				throw ModelException(__LINE__, __FILE__, oss.str());
			}

			// Parse the last three values in the line as floats and add a new vertex to the vertex positions vector
			positions.push_back(XMFLOAT3());
			positions.back().x = std::stof(lineTokens[1]);
			positions.back().y = std::stof(lineTokens[2]);
			positions.back().z = std::stof(lineTokens[3]);
		}
		// If token is 'vt', the next 2 values in the line are for a texture coordinate
		else if (lineTokens[0] == "vt")
		{
			// Make sure the number of tokens in the line is exactly 3
			if (lineTokens.size() != 3)
			{
				std::ostringstream oss;
				oss << "File '" << filename << "' has invalid number of texture coordinate parameters at line " << lineNumber;
				throw ModelException(__LINE__, __FILE__, oss.str());
			}

			// Parse the last three values in the line as floats and add a new vertex to the vertex positions vector
			textures.push_back(XMFLOAT2());
			textures.back().x = std::stof(lineTokens[1]);
			textures.back().y = std::stof(lineTokens[2]);
		}
		// If token is 'vn', the next 3 values in the line are for a vertex normal
		else if (lineTokens[0] == "vn")
		{
			// Make sure the number of tokens in the line is exactly 4
			if (lineTokens.size() != 4)
			{
				std::ostringstream oss;
				oss << "File '" << filename << "' has invalid number of vertex normal parameters at line " << lineNumber;
				throw ModelException(__LINE__, __FILE__, oss.str());
			}

			// Parse the last three values in the line as floats and add a new vertex to the vertex positions vector
			normals.push_back(XMFLOAT3());
			normals.back().x = std::stof(lineTokens[1]);
			normals.back().y = std::stof(lineTokens[2]);
			normals.back().z = std::stof(lineTokens[3]);
		}
	}

	file.close();
}

void Model::OBJCreateVertices(std::string filename, std::vector<DirectX::XMFLOAT3>& positions, std::vector<DirectX::XMFLOAT2>& textures, std::vector<DirectX::XMFLOAT3>& normals)
{
	// Create the root node - will initially have no data and no name
	m_rootNode = std::make_unique<ModelNode>(m_deviceResources, m_moveLookController);
	
	std::vector<OBJVertex> vertices;		// final vertices for the vertex buffer
	std::vector<unsigned short> indices;	// final indices for the index buffer
	std::string childNodeName;				// string to hold the name of the child we are currently gathering data on

	// Create the lookup vector for position data - fill all values with -1
	std::vector<int> indexLookup(positions.size());
	std::fill(indexLookup.begin(), indexLookup.end(), -1);

	// Open the file -------------------------------------------------
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::ostringstream oss;
		oss << "Could not open file: " << filename;
		throw ModelException(__LINE__, __FILE__, oss.str());
	}

	// Parsing variables ---------------------------------------------
	int lineNumber = 0;						// Used in exception messages for debugging
	std::vector<std::string> lineTokens;	// vector to hold each string parsed from a line
	std::istringstream lineISS;				// istringstream for parsing line tokens
	std::string lineItem;					// temporary string to hold parsed token to be added to token vector

	std::vector<std::string> faceTokens;	// vector to hold tokenized v/vt/vn values
	std::istringstream faceISS;				// istringstream for parsing v/vt/vn values
	std::string faceItem;					// temporary string to hold parsed v/vt/vn values to be added to the token vector

	unsigned short parsedPositionIndex, parsedTextureIndex, parsedNormalIndex; // parsed index values for the 

	// Loop over each line in the file -------------------------------
	std::string line;
	while (std::getline(file, line))
	{
		// Update the line number
		++lineNumber;

		// Get each string in the line, splitting on space character ' '
		lineTokens.clear();
		lineISS = std::istringstream(line);
		while (std::getline(lineISS, lineItem, ' '))
			lineTokens.push_back(lineItem);

		if (lineTokens.size() == 0)
			continue;

		// If token is 'o', the next value is the name of the node
		if (lineTokens[0] == "o")
		{
			// Make sure the number of tokens in the line is exactly 2
			if (lineTokens.size() != 2)
			{
				std::ostringstream oss;
				oss << "File '" << filename << "' has invalid number of node name parameters at line " << lineNumber;
				throw ModelException(__LINE__, __FILE__, oss.str());
			}

			// If no vertices have been gathered yet, then this must be the first 'o' in the file, which means it is for the root node
			if (vertices.size() == 0)
				m_rootNode->SetName(lineTokens[1]);
			else
			{
				// if the mesh for the root node is still nullptr, then the data that has been gathered belongs to the root node
				if (m_rootNode->GetMesh() == nullptr)
					m_rootNode->CreateMesh(vertices, indices);
				// The root node has already been created, so add the data as a child node
				else
					m_rootNode->CreateChildNode(childNodeName, vertices, indices);

				// keep track of the name of the next node, which will be a child node
				childNodeName = lineTokens[1];

				// clean up the data for the vertices and indices
				vertices.clear();
				indices.clear();
			}			
		}
		// If token is 'f', the next 3 values in the line are for the v/vt/vn values
		else if (lineTokens[0] == "f")
		{
			// Make sure the number of tokens in the line is exactly 4
			if (lineTokens.size() != 4)
			{
				std::ostringstream oss;
				oss << "File '" << filename << "' has invalid number of face parameters at line " << lineNumber;
				throw ModelException(__LINE__, __FILE__, oss.str());
			}

			// Iterate over each of the three v/vt/vn values
			for (int iii = 1; iii <= 3; ++iii)
			{
				faceTokens.clear();
				faceISS = std::istringstream(lineTokens[iii]);
				while (std::getline(faceISS, faceItem, '/'))
					faceTokens.push_back(faceItem);

				if (faceTokens.size() != 3)
				{
					std::ostringstream oss;
					oss << "File '" << filename << "' has invalid number of face tokens at line " << lineNumber;
					throw ModelException(__LINE__, __FILE__, oss.str());
				}

				parsedPositionIndex = static_cast<unsigned short>(std::stoi(faceTokens[0]) - 1); // OBJ is NOT 0 indexed - index starts at 1 so we need to subtract 1 here
				parsedTextureIndex = static_cast<unsigned short>(std::stoi(faceTokens[1]) - 1);
				parsedNormalIndex = static_cast<unsigned short>(std::stoi(faceTokens[2]) - 1);

				// If the position was not used to create a previous vertex, then create a new one
				if (indexLookup[parsedPositionIndex] == -1)
				{
					// Create and add new vertex
					vertices.push_back(OBJVertex());
					vertices.back().position = positions[parsedPositionIndex];
					vertices.back().texture = textures[parsedTextureIndex];
					vertices.back().normal = normals[parsedNormalIndex];

					// Add new index for the vertex
					indices.push_back(static_cast<unsigned short>(vertices.size() - 1));

					// Update the lookup list so we know not to create a duplicate vertex for this position
					indexLookup[parsedPositionIndex] = static_cast<int>(vertices.size() - 1);
				}
				else
				{
					// Vertex was already added - just add the index
					indices.push_back(static_cast<unsigned short>(indexLookup[parsedPositionIndex]));
				}
			}
		}
	}

	// if the mesh for the root node is still nullptr, then the data that has been gathered belongs to the root node
	if (m_rootNode->GetMesh() == nullptr)
		m_rootNode->CreateMesh(vertices, indices);
	// The root node has already been created, so add the data as a child node
	else
		m_rootNode->CreateChildNode(childNodeName, vertices, indices);

	// clean up the data for the vertices and indices
	vertices.clear();
	indices.clear();

	file.close();
}


void Model::Draw(XMMATRIX parentModelMatrix, XMMATRIX projectionMatrix)
{
	// Draw the root node applying no transformations yet
	m_rootNode->Draw(parentModelMatrix, projectionMatrix);
}

