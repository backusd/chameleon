#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "Mesh.h"
#include "ObjectStore.h"
#include "Renderable.h"
#include "StepTimer.h"
#include "HLSLStructures.h"


#include <string>
#include <vector>

/*
A DrawPipeline will be configured for each item(s) that need to be rendered. It will consist of the following:
	1 Mesh
	1+ Renderables that will use the same mesh
	1 Vertex Shader
	1 Pixel Shader


A Draw call on the DrawPipeline will do the following:
	* Attach the Vertex Shader to the pipeline
	* Attach the Pixel Shader to the pipeline
	* Call Mesh->PreparePipeline()
	* For each renderable:
		* Update the vertex buffer
		* Issue the draw call (get the index count from the mesh)
	
*/

class DrawPipeline
{
public:
	DrawPipeline(std::shared_ptr<DeviceResources> deviceResources, 
		std::string meshName,
		std::string vertexShaderName, 
		std::string pixelShaderName);
	DrawPipeline(std::shared_ptr<DeviceResources> deviceResources,
		std::string meshName,
		std::string vertexShaderName,
		std::string pixelShaderName,
		std::vector<std::string> vertexShaderConstantBufferNames,
		std::vector<std::string> pixelShaderConstantBufferNames);

	void Update(std::shared_ptr<StepTimer> timer);
	void Draw(DirectX::XMMATRIX viewProjection);


	void AddRenderable(std::shared_ptr<Renderable> renderable) { m_renderables.push_back(renderable); }

	void UpdatePSSubresource(int index, void* data);
	void UpdateVSSubresource(int index, void* data);

private:
	void SetPSConstantBuffers();
	void SetVSConstantBuffers();

	std::shared_ptr<DeviceResources> m_deviceResources;

	std::shared_ptr<Mesh> m_mesh;

	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_pixelShaderConstantBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_vertexShaderConstantBuffers;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

	std::vector<std::shared_ptr<Renderable>> m_renderables;
};