#pragma once
#include "pch.h"

#include "DeviceResources.h"
#include "CameraClass.h"
#include "PositionClass.h"
#include "LightClass.h"
#include "SkyDomeClass.h"

#include <memory>



///////////////////////
// MY CLASS INCLUDES //
///////////////////////
/*
#include "d3dclass.h"
#include "inputclass.h"
#include "cameraclass.h"
#include "positionclass.h"
#include "lightclass.h"
#include "skydomeclass.h"
#include "skydomeshaderclass.h"
#include "terrainclass.h"
#include "terrainshaderclass.h"
#include "textureshaderclass.h"
#include "modelclass.h"
#include "entitylistclass.h"
#include "frustumclass.h"
#include "quadtreeclass.h"
#include "userinterfaceclass.h"
*/


////////////////////////////////////////////////////////////////////////////////
// Class name: BlackForestClass
////////////////////////////////////////////////////////////////////////////////
class BlackForestClass
{
public:
	BlackForestClass(std::shared_ptr<DeviceResources> deviceResources, HWND hwnd, int screenWidth, int screenHeight, float screenDepth, float screenNear);
	~BlackForestClass();

	void AddEntity(unsigned short, char, float, float, float, float, float, float);
	void RemoveEntity(unsigned short);
	void UpdateEntityState(unsigned short, char);
	void UpdateEntityPosition(unsigned short, float, float, float, float, float, float);
	void UpdateEntityRotate(unsigned short, bool);
	/*
	bool Initialize(D3DClass*, HWND, int, int, float, float);
	void Shutdown();
	bool Frame(D3DClass*, InputClass*, float, UserInterfaceClass*);

	
	bool GetStateChange(char&);
	
	bool PositionUpdate(float&, float&, float&, float&, float&, float&);
	
	*/

private:
	std::shared_ptr<DeviceResources> m_deviceResources;

	std::shared_ptr<CameraClass> m_Camera;
	std::shared_ptr<PositionClass> m_Position;
	std::shared_ptr<LightClass> m_Light;
	std::shared_ptr<SkyDomeClass> m_SkyDome;

	/*
	void HandleMovementInput(InputClass*, float);
	bool Render(D3DClass*, UserInterfaceClass*);

	SkyDomeShaderClass* m_SkyDomeShader;
	TerrainClass* m_Terrain;
	TerrainShaderClass* m_TerrainShader;
	TextureShaderClass* m_TextureShader;
	ModelClass* m_CubeModel1, * m_CubeModel2;
	EntityListClass* m_EntityList;
	FrustumClass* m_Frustum;
	
	QuadTreeClass* m_QuadTree;
	*/
	float m_screenDepth;
	bool m_stateChange;
	char m_newState;
	unsigned long m_updateTime;
	bool m_positionUpdateReady;
};