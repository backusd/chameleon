#include "Texture.h"


Texture::Texture(std::shared_ptr<DeviceResources> deviceResources, std::string filename) :
	m_deviceResources(deviceResources)
{
	INFOMAN(m_deviceResources);

	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// Load the targa image data into memory.
	LoadTarga(filename, height, width);

	// Setup the description of the texture.
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateTexture2D(&textureDesc, NULL, &m_texture));

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	m_deviceResources->D3DDeviceContext()->UpdateSubresource(m_texture.Get(), 0, NULL, m_targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_textureView.ReleaseAndGetAddressOf())
	);

	// Generate mipmaps for this texture.
	m_deviceResources->D3DDeviceContext()->GenerateMips(m_textureView.Get());

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] m_targaData;
	m_targaData = 0;
}

void Texture::LoadTarga(std::string filename, int& height, int& width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;
	std::ostringstream oss;

	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename.c_str(), "rb");
	if (error != 0)
	{
		oss << "Failed to open file: " << filename;
		throw TextureException(__LINE__, __FILE__, oss.str());
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		oss << "Failed to read in the file header for file: " << filename;
		throw TextureException(__LINE__, __FILE__, oss.str());
	}

	// Get the important information from the header.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		oss << "bpp is NOT 32 for file: " << filename << std::endl;
		oss << "    bpp = " << bpp;
		throw TextureException(__LINE__, __FILE__, oss.str());
	}

	// Calculate the size of the 32 bit image data.
	imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		oss << "Failed to read in expected amount of data:" << std::endl;
		oss << "    File:     " << filename << std::endl;
		oss << "    Expected: " << imageSize << std::endl;
		oss << "    Actual:   " << count << std::endl;
		throw TextureException(__LINE__, __FILE__, oss.str());
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		oss << "Failed to close the file: " << filename;
		throw TextureException(__LINE__, __FILE__, oss.str());
	}

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;
}