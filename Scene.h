#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "DeviceResourcesException.h"
#include "ObjectStore.h"
#include "DrawPipeline.h"
#include "Box.h"
#include "StepTimer.h"
#include "HLSLStructures.h"
#include "MoveLookController.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Terrain.h"
#include "SkyDomeMesh.h"
#include "TerrainMesh.h"
#include "Frustum.h"

#include <memory>

class Scene
{
public:
	class Scene(std::shared_ptr<DeviceResources> deviceResources, HWND hWnd);
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	void Update(std::shared_ptr<StepTimer> timer, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse);
	void Draw();


	DirectX::XMMATRIX ViewProjectionMatrix() { return m_viewMatrix * m_projectionMatrix; }
	DirectX::XMMATRIX ViewMatrix() { return m_viewMatrix; }
	DirectX::XMMATRIX ProjectionMatrix() { return m_projectionMatrix; }
	

private:
	void CreateStaticResources();
	void CreateWindowSizeDependentResources();

	void SetupCubePipeline();
	void SetupTerrainPipeline();
	void SetupTerrainCubePipeline();
	void SetupSkyDomePipeline();

	PhongMaterialProperties* m_material;

	HWND m_hWnd;
	std::shared_ptr<DeviceResources> m_deviceResources;

	std::shared_ptr<DrawPipeline> m_cubePipeline;
	std::shared_ptr<DrawPipeline> m_skyDomePipeline;
	
	std::vector<bool> m_terrainCellVisibility;
	std::vector<std::shared_ptr<DrawPipeline>> m_terrainPipelines;
	std::shared_ptr<DrawPipeline> m_terrainCubePipeline;

	std::unique_ptr<MoveLookController> m_moveLookController;
	std::shared_ptr<Frustum> m_frustum;

	CD3D11_VIEWPORT m_viewport;

	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_modelViewProjectionBuffer;
	ModelViewProjectionConstantBuffer			m_modelViewProjectionBufferData;
	DirectX::XMMATRIX							m_viewMatrix;
	DirectX::XMMATRIX							m_projectionMatrix;
	DirectX::XMMATRIX							m_viewProjectionMatrix;

	// Light Properties
	LightProperties								m_lightProperties;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_lightPropertiesConstantBuffer;

	// MaterialProperties
	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_phongMaterialPropertiesConstantBuffer;
	std::vector<PhongMaterialProperties*>		m_phongMaterialProperties;
	std::unique_ptr<PhongMaterialProperties>	m_velocityArrowMaterial;

	Microsoft::WRL::ComPtr<ID3D11Buffer>		m_solidMaterialPropertiesConstantBuffer;
};


/*
IASetIndexBuffer
IASetInputLayout
IASetPrimitiveTopology
IASetVertexBuffers

VSSetConstantBuffers
VSSetSamplers
VSSetShader
VSSetShaderResources

PSSetConstantBuffers
PSSetSamplers
PSSetShader
PSSetShaderResources



*/