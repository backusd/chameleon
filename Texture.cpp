#include "Texture.h"


Texture::Texture(std::shared_ptr<DeviceResources> deviceResources) :
	m_deviceResources(deviceResources)
{
	Reset();
}

void Texture::Reset()
{
	// Load default texture and SRV values
	// (don't load height/width because the Load* function that loads the image file will take care of that)
	m_textureDesc.MipLevels = 0;
	m_textureDesc.ArraySize = 1;
	m_textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_textureDesc.SampleDesc.Count = 1;
	m_textureDesc.SampleDesc.Quality = 0;
	m_textureDesc.Usage = D3D11_USAGE_DEFAULT;
	m_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	m_textureDesc.CPUAccessFlags = 0;
	m_textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	m_srvDesc.Format = m_textureDesc.Format;
	m_srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	m_srvDesc.Texture2D.MostDetailedMip = 0;
	m_srvDesc.Texture2D.MipLevels = -1;
}

void Texture::LoadTarga(std::string filename)
{
	INFOMAN(m_deviceResources);

	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;
	unsigned char* targaData;
	std::ostringstream oss;
	int height, width;

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
	targaData = new unsigned char[imageSize];

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (width * height * 4) - (width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down.
	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
		{
			targaData[index + 0] = targaImage[k + 2];  // Red.
			targaData[index + 1] = targaImage[k + 1];  // Green.
			targaData[index + 2] = targaImage[k + 0];  // Blue
			targaData[index + 3] = targaImage[k + 3];  // Alpha

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

	// ===================================================================
	// Now load the data into the texture
	m_textureDesc.Height = height;
	m_textureDesc.Width = width;

	// Create the empty texture.
	GFX_THROW_INFO(m_deviceResources->D3DDevice()->CreateTexture2D(&m_textureDesc, NULL, &m_texture));

	// Set the row pitch of the targa image data.
	unsigned int rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->UpdateSubresource(m_texture.Get(), 0, NULL, targaData, rowPitch, 0)
	);


	// Create the shader resource view for the texture.
	GFX_THROW_INFO(
		m_deviceResources->D3DDevice()->CreateShaderResourceView(m_texture.Get(), &m_srvDesc, m_textureView.ReleaseAndGetAddressOf())
	);

	// Generate mipmaps for this texture.
	GFX_THROW_INFO_ONLY(
		m_deviceResources->D3DDeviceContext()->GenerateMips(m_textureView.Get())
	);
}