#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "Mesh.h"
#include "ObjectStore.h"
#include "Renderable.h"
#include "StepTimer.h"
#include "HLSLStructures.h"
#include "Texture.h"


#include <string>
#include <vector>
#include <functional>

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
		std::string pixelShaderName,
		std::string rasterStateName,
		std::string depthStencilStateName);
	DrawPipeline(std::shared_ptr<DeviceResources> deviceResources,
		std::string meshName,
		std::string vertexShaderName,
		std::string pixelShaderName,
		std::string rasterStateName,
		std::string depthStencilStateName,
		std::string vertexShaderConstantBufferArrayName);
	DrawPipeline(std::shared_ptr<DeviceResources> deviceResources,
		std::string meshName,
		std::string vertexShaderName,
		std::string pixelShaderName,
		std::string rasterStateName,
		std::string depthStencilStateName,
		std::string vertexShaderConstantBufferArrayName,
		std::string pixelShaderConstantBufferArrayName);

	void Update(std::shared_ptr<StepTimer> timer);
	void Draw();


	void AddRenderable(std::shared_ptr<Renderable> renderable) { m_renderables.push_back(renderable); }
	
	void AddPixelShaderTexture(std::string textureLookupName);
	void SetSamplerState(std::string sampleStateLookupName);

	std::shared_ptr<Renderable> GetRenderable(int index) { return m_renderables[index]; }


	void UpdatePSSubresource(int index, void* data);
	void UpdateVSSubresource(int index, void* data);

	void SetPerRendererableUpdate(std::function<void(std::shared_ptr<Renderable> renderable,
													 std::shared_ptr<Mesh> mesh,
													 std::shared_ptr<ConstantBufferArray>,
													 std::shared_ptr<ConstantBufferArray>)> function) { PerRendererableUpdate = function; }

private:
	std::function<void(std::shared_ptr<Renderable> renderable,
						std::shared_ptr<Mesh> mesh,
						std::shared_ptr<ConstantBufferArray> vertexShaderBufferArray,
						std::shared_ptr<ConstantBufferArray> pixelShaderBufferArray)> PerRendererableUpdate;


	std::shared_ptr<DeviceResources> m_deviceResources;

	std::shared_ptr<Mesh> m_mesh;

	std::shared_ptr<ConstantBufferArray> m_vertexShaderConstantBufferArray;
	std::shared_ptr<ConstantBufferArray> m_pixelShaderConstantBufferArray;

	
	
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			m_samplerState;


	std::shared_ptr<InputLayout>	m_inputLayout;
	std::shared_ptr<VertexShader>	m_vertexShader;
	std::shared_ptr<PixelShader>	m_pixelShader;	
	
	std::shared_ptr<RasterizerState> m_rasterizerState;

	std::vector<std::shared_ptr<Texture>>		m_pixelShaderTextures;
	std::vector<std::shared_ptr<Renderable>>	m_renderables;

	std::shared_ptr<DepthStencilState>			m_depthStencilState;
};