#include "ContentWindow.h"

using Microsoft::WRL::ComPtr;
using DirectX::XMFLOAT3;


ContentWindow::ContentWindow(int width, int height, const char* name) :
	WindowBase(width, height, name),
	// m_stateBlock(nullptr),
	m_io(ImGui::GetIO()),
	m_timer(nullptr),
	m_cpu(nullptr),
	m_keyboard(nullptr),
	m_mouse(nullptr),
	m_network(nullptr),
	m_hud(nullptr),
	m_scene(nullptr)
{
	// Create the device resources
	m_deviceResources = std::make_shared<DeviceResources>(m_hWnd);
	m_deviceResources->OnResize(); // Calling OnResize will create the render target, etc.

	

	(void)m_io;
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	m_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (m_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(m_deviceResources->D3DDevice(), m_deviceResources->D3DDeviceContext());

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);


	// Our state
	m_show_demo_window = true;
	m_show_another_window = false;
	m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// We now have access to the device, so we now need to initialize the object store before creating the scene
	ObjectStore::Initialize(m_deviceResources);

	ObjectStoreAddShaders();
	ObjectStoreAddTerrains(); // Must call this first because ObjectStoreAddMeshes tries to lookup the terrains
	ObjectStoreAddMeshes();
	ObjectStoreAddConstantBuffers();
	ObjectStoreAddRasterStates();
	ObjectStoreAddSamplerStates();
	ObjectStoreAddTextures();
	ObjectStoreAddDepthStencilStates();


	m_timer = std::make_shared<StepTimer>();
	m_cpu = std::make_shared<CPU>(m_timer);

	m_keyboard = std::make_shared<Keyboard>();
	m_mouse = std::make_shared<Mouse>();

	// m_network = std::make_shared<Network>("155.248.215.180", 7000, m_timer);

	m_hud = std::make_shared<HUD>(m_deviceResources);
	m_scene = std::make_shared<Scene>(m_deviceResources, m_hWnd);	

	//
	// Consider making this NDEBUG only
	//
	m_enableImGuiWindows = true;
}

ContentWindow::~ContentWindow()
{
	// Have to make sure to delete objects on close
	ObjectStore::DestructObjects();
}

void ContentWindow::ObjectStoreAddShaders()
{
	// Basic Cube ==================================================================================================
	std::shared_ptr<InputLayout> basicCubeLayout = std::make_shared<InputLayout>(m_deviceResources, L"VertexShader.cso");
	basicCubeLayout->AddDescription("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	basicCubeLayout->CreateLayout();

	ObjectStore::AddBindable("basic-cube-vertex-shader-IA", basicCubeLayout);
	ObjectStore::AddBindable("basic-cube-vertex-shader", std::make_shared<VertexShader>(m_deviceResources, basicCubeLayout->GetVertexShaderFileBlob()));
	ObjectStore::AddBindable("basic-cube-pixel-shader", std::make_shared<PixelShader>(m_deviceResources, L"PixelShader.cso"));

	// Terrain =====================================================================================================
	std::shared_ptr<InputLayout> terrainLayout = std::make_shared<InputLayout>(m_deviceResources, L"TerrainVertexShader.cso");
	terrainLayout->AddDescription("POSITION", 0,    DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	terrainLayout->AddDescription(   "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	terrainLayout->CreateLayout();

	ObjectStore::AddBindable("terrain-vertex-shader-IA", terrainLayout);
	ObjectStore::AddBindable("terrain-vertex-shader", std::make_shared<VertexShader>(m_deviceResources, terrainLayout->GetVertexShaderFileBlob()));
	ObjectStore::AddBindable("terrain-pixel-shader", std::make_shared<PixelShader>(m_deviceResources, L"TerrainPixelShader.cso"));

	// Terrain Texture ==============================================================================================
	std::shared_ptr<InputLayout> terrainTextureLayout = std::make_shared<InputLayout>(m_deviceResources, L"TerrainTextureVertexShader.cso");
	terrainTextureLayout->AddDescription("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	terrainTextureLayout->AddDescription("TEXCOORD", 0,    DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	terrainTextureLayout->AddDescription(  "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	terrainTextureLayout->AddDescription( "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	terrainTextureLayout->AddDescription("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	terrainTextureLayout->AddDescription(   "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	terrainTextureLayout->CreateLayout();

	ObjectStore::AddBindable("terrain-texture-vertex-shader-IA", terrainTextureLayout);
	ObjectStore::AddBindable("terrain-texture-vertex-shader", std::make_shared<VertexShader>(m_deviceResources, terrainTextureLayout->GetVertexShaderFileBlob()));
	ObjectStore::AddBindable("terrain-texture-pixel-shader", std::make_shared<PixelShader>(m_deviceResources, L"TerrainTexturePixelShader.cso"));


	// Phong ======================================================================================================
	std::shared_ptr<InputLayout> phongLayout = std::make_shared<InputLayout>(m_deviceResources, L"PhongVertexShader.cso");
	phongLayout->AddDescription("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	phongLayout->AddDescription(  "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	phongLayout->CreateLayout();

	ObjectStore::AddBindable("phong-vertex-shader-IA", phongLayout);
	ObjectStore::AddBindable("phong-vertex-shader", std::make_shared<VertexShader>(m_deviceResources, phongLayout->GetVertexShaderFileBlob()));
	ObjectStore::AddBindable("phong-pixel-shader", std::make_shared<PixelShader>(m_deviceResources, L"PhongPixelShader.cso"));


	// Sky Dome ======================================================================================================
	std::shared_ptr<InputLayout> skyDomeLayout = std::make_shared<InputLayout>(m_deviceResources, L"SkyDomeVertexShader.cso");
	skyDomeLayout->AddDescription("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	skyDomeLayout->CreateLayout();

	ObjectStore::AddBindable("sky-dome-vertex-shader-IA", skyDomeLayout);
	ObjectStore::AddBindable("sky-dome-vertex-shader", std::make_shared<VertexShader>(m_deviceResources, skyDomeLayout->GetVertexShaderFileBlob()));
	ObjectStore::AddBindable("sky-dome-pixel-shader", std::make_shared<PixelShader>(m_deviceResources, L"SkyDomePixelShader.cso"));


	// Solid color =====================================================================================================
	std::shared_ptr<InputLayout> solidColorLayout = std::make_shared<InputLayout>(m_deviceResources, L"SolidVertexShader.cso");
	solidColorLayout->AddDescription("POSITION", 0,    DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	solidColorLayout->AddDescription(   "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0);
	solidColorLayout->CreateLayout();

	ObjectStore::AddBindable("solid-vertex-shader-IA", solidColorLayout);
	ObjectStore::AddBindable("solid-vertex-shader", std::make_shared<VertexShader>(m_deviceResources, solidColorLayout->GetVertexShaderFileBlob()));
	ObjectStore::AddBindable("solid-pixel-shader", std::make_shared<PixelShader>(m_deviceResources, L"SolidPixelShader.cso"));
}
void ContentWindow::ObjectStoreAddTerrains()
{
	ObjectStore::AddTerrainMesh(std::make_shared<TerrainMesh>(m_deviceResources), "terrain-mesh");
}
void ContentWindow::ObjectStoreAddMeshes()
{
	ObjectStore::AddMesh("box-filled-mesh", std::make_shared<BoxMesh>(m_deviceResources, true));
	ObjectStore::AddMesh("box-outline-mesh", std::make_shared<BoxMesh>(m_deviceResources, false));
	ObjectStore::AddMesh("sky-dome-mesh", std::make_shared<SkyDomeMesh>(m_deviceResources));

	// Add terrain cell meshes sequentially
	std::shared_ptr<TerrainMesh> terrain = ObjectStore::GetTerrainMesh("terrain-mesh");
	for (int iii = 0; iii < terrain->TerrainCellCount(); ++iii)
	{
		std::ostringstream oss;
		oss << "terrain_" << iii;
		ObjectStore::AddMesh(oss.str(), terrain->GetTerrainCell(iii));
	}
}
void ContentWindow::ObjectStoreAddConstantBuffers()
{
	std::shared_ptr<ConstantBuffer> b1 = std::make_shared<ConstantBuffer>(m_deviceResources);
	std::shared_ptr<ConstantBuffer> b2 = std::make_shared<ConstantBuffer>(m_deviceResources);
	std::shared_ptr<ConstantBuffer> b3 = std::make_shared<ConstantBuffer>(m_deviceResources);
	std::shared_ptr<ConstantBuffer> b5 = std::make_shared<ConstantBuffer>(m_deviceResources);
	std::shared_ptr<ConstantBuffer> b7 = std::make_shared<ConstantBuffer>(m_deviceResources);

	b1->CreateBuffer<ModelViewProjectionConstantBuffer>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0, 0);
	b2->CreateBuffer<PhongMaterialProperties>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0, 0);
	b3->CreateBuffer<LightProperties>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0, 0);
	b5->CreateBuffer<TerrainLightBufferType>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0, 0);
	b7->CreateBuffer<SkyDomeColorBufferType>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0, 0);

	ObjectStore::AddConstantBuffer("model-view-projection-buffer", b1);
	ObjectStore::AddConstantBuffer("phong-material-properties-buffer", b2);
	ObjectStore::AddConstantBuffer("light-properties-buffer", b3);
	ObjectStore::AddConstantBuffer("terrain-light-buffer", b5);
	ObjectStore::AddConstantBuffer("sky-dome-gradient-buffer", b7);

	// Now create the buffer arrays

	// Cube - VS
	//std::shared_ptr<ConstantBufferArray> ba1 = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::VERTEX_SHADER);
	//ba1->AddBuffer("model-view-projection-buffer");
	//ObjectStore::AddBindable("cube-buffers-VS", ba1);

	// Cube - PS
	//std::shared_ptr<ConstantBufferArray> ba2 = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::PIXEL_SHADER);
	//ba2->AddBuffer("phong-material-properties-buffer");
	//ba2->AddBuffer("light-properties-buffer");
	//ObjectStore::AddBindable("cube-buffers-PS", ba2);

	// Terrain - VS
	//std::shared_ptr<ConstantBufferArray> ba3 = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::VERTEX_SHADER);
	//ba3->AddBuffer("terrain-constant-buffer");
	//ObjectStore::AddBindable("terrain-buffers-VS", ba3);

	// Terrain - PS
	std::shared_ptr<ConstantBufferArray> ba4 = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::PIXEL_SHADER);
	ba4->AddBuffer("terrain-light-buffer");
	ObjectStore::AddBindable("terrain-buffers-PS", ba4);


	// Terrain Cube - VS
	//std::shared_ptr<ConstantBufferArray> ba5 = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::VERTEX_SHADER);
	//ba5->AddBuffer("terrain-constant-buffer");
	//ObjectStore::AddBindable("terrain-cube-buffers-VS", ba5);


	// Sky Dome - VS
	//std::shared_ptr<ConstantBufferArray> ba6 = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::VERTEX_SHADER);
	//ba6->AddBuffer("sky-dome-constant-buffer");
	//ObjectStore::AddBindable("sky-dome-buffers-VS", ba6);

	// Sky Dome - PS
	//std::shared_ptr<ConstantBufferArray> ba7 = std::make_shared<ConstantBufferArray>(m_deviceResources, ConstantBufferBindingLocation::PIXEL_SHADER);
	//ba7->AddBuffer("sky-dome-gradient-buffer");
	//ObjectStore::AddBindable("sky-dome-buffers-PS", ba7);
}
void ContentWindow::ObjectStoreAddRasterStates()
{
	std::shared_ptr<RasterizerState> solidRS = std::make_shared<RasterizerState>(m_deviceResources);
	ObjectStore::AddBindable("solidfill", solidRS);

	std::shared_ptr<RasterizerState> wireframeRS = std::make_shared<RasterizerState>(m_deviceResources);
	wireframeRS->FillMode(D3D11_FILL_WIREFRAME);
	ObjectStore::AddBindable("wireframe", wireframeRS);
}
void ContentWindow::ObjectStoreAddSamplerStates()
{
	// This will just use the default values for a sampler, but all values can be customized
	std::shared_ptr<SamplerState> sampler = std::make_shared<SamplerState>(m_deviceResources);
	ObjectStore::AddBindable("terrain-texture-sampler", sampler);
}
void ContentWindow::ObjectStoreAddTextures()
{
	// MAKE SURE TO MODIFY THE TEXTURE DESCRIPTION STRUCT COMPLETELY PRIOR TO CALLING A LOAD* FUNCTION

	std::shared_ptr<Texture> dirt = std::make_shared<Texture>(m_deviceResources);
	dirt->Create("dirt01d.tga");
	ObjectStore::AddTexture("terrain-texture", dirt);

	std::shared_ptr<Texture> dirtNormals = std::make_shared<Texture>(m_deviceResources);
	dirtNormals->Create("dirt01n.tga");
	ObjectStore::AddTexture("terrain-normal-map-texture", dirtNormals);


	// Now create a texture array that can be bound to the pipeline

	std::shared_ptr<TextureArray> dirtTextureArray = std::make_shared<TextureArray>(m_deviceResources, TextureBindingLocation::PIXEL_SHADER);
	dirtTextureArray->AddTexture("terrain-texture");
	dirtTextureArray->AddTexture("terrain-normal-map-texture");
	ObjectStore::AddBindable("dirt-terrain-texture-array", dirtTextureArray);

}
void ContentWindow::ObjectStoreAddDepthStencilStates()
{
	std::shared_ptr<DepthStencilState> depthEnabled = std::make_shared<DepthStencilState>(m_deviceResources, 1);
	ObjectStore::AddBindable("depth-enabled-depth-stencil-state", depthEnabled);

	std::shared_ptr<DepthStencilState> depthDisabled = std::make_shared<DepthStencilState>(m_deviceResources, 1);
	depthDisabled->DepthEnable(false);
	ObjectStore::AddBindable("depth-disabled-depth-stencil-state", depthDisabled);
}

void ContentWindow::Update()
{
	// while (!m_keyboard->CharIsEmpty())
	//	oss << m_keyboard->ReadChar();
	// SetWindowText(m_hWnd, oss.str().c_str());

	//std::ostringstream oss;
	//oss << "Time: " << m_timer->GetTotalSeconds();
	//SetWindowText(m_hWnd, oss.str().c_str());

	/*
	std::ostringstream oss;
	if (m_mouse->LeftIsPressed())
		oss << "Left";
	else
		oss << "(" << m_mouse->GetPosX() << ", " << m_mouse->GetPosY() << ")";
	SetWindowText(m_hWnd, oss.str().c_str());
	*/

	m_timer->Tick([&]()
		{
			m_cpu->Update();

			// m_network->Update();

			// Don't update keyboard or mouse because they get updated via windows messages

			//m_network->Update();

			m_hud->Update(m_timer);
			m_scene->Update(m_timer, m_keyboard, m_mouse);

			/*
			m_cpuStatistics->Update(m_timer);

			m_inputClass->Frame();

			// Do the network frame processing.
			m_network->Frame();


			// Do the UI frame processing.
			//result = m_UserInterface->Frame(m_D3D, m_Input, m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), m_Network->GetLatency());
			//if (!result)
			//{
			//	return false;
			//}

			switch (m_stateClass->GetCurrentState())
			{
			case STATE_BLACKFOREST:
				
				result = m_BlackForest->Frame(m_D3D, m_Input, m_Timer->GetTime(), m_UserInterface);

				// Check for state changes.
				if (m_BlackForest->GetStateChange(state) == true)
				{
					m_Network->SendStateChange(state);
				}

				// Check for position updates.
				if (m_BlackForest->PositionUpdate(posX, posY, posZ, rotX, rotY, rotZ) == true)
				{
					m_Network->SendPositionUpdate(posX, posY, posZ, rotX, rotY, rotZ);
				}
				
				break;

			default:
				break;
			}*/
			
		}
	);
}

bool ContentWindow::Render()
{
	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	// Draw the scene before rendering the ImGui controls so you have the option to have ImGui rendered over the scene
	m_scene->Draw();


	// Start the Dear ImGui frame =========================================================================================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Render Stats ================================================================================================
	ImGui::Begin("Render Stats");
	ImGui::Checkbox("Enable all other ImGui windows", &m_enableImGuiWindows);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / m_io.Framerate, m_io.Framerate);
	ImGui::End();

	// Have the scene draw the necessary ImGui controls ==============================================================
	if (m_enableImGuiWindows)
		m_scene->DrawImGui();


	// Render ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (m_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	return true;
}

void ContentWindow::Present()
{
	// Present the render target to the screen
	m_deviceResources->Present();
}

void ContentWindow::Destroy()
{
	/*
	// Release the black forest object.
	if (m_BlackForest)
	{
		m_BlackForest->Shutdown();
		delete m_BlackForest;
		m_BlackForest = 0;
	}

	// Release the StateClass object.
	if (m_State)
	{
		delete m_State;
		m_State = 0;
	}

	// Release the user interface object.
	if (m_UserInterface)
	{
		m_UserInterface->Shutdown();
		delete m_UserInterface;
		m_UserInterface = 0;
	}

	// Release the cpu object.
	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the network object.
	if (m_Network)
	{
		m_Network->Shutdown();
		delete m_Network;
		m_Network = 0;
	}

	// Release the D3D object.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	*/
}






LRESULT ContentWindow::OnCreate(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return 0;
}

LRESULT ContentWindow::OnDestroy(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	DiscardGraphicsResources();
	return 0;
}

LRESULT ContentWindow::OnLButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	m_mouse->OnLeftPressed(pt.x, pt.y);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	m_mouse->OnLeftReleased(pt.x, pt.y);
	// release mouse if outside of window
	if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
	{
		ReleaseCapture();
		m_mouse->OnMouseLeave();
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnLButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	m_mouse->OnLeftDoubleClick(pt.x, pt.y);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	m_mouse->OnMiddlePressed(pt.x, pt.y);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	m_mouse->OnMiddleReleased(pt.x, pt.y);
	// release mouse if outside of window
	if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
	{
		ReleaseCapture();
		m_mouse->OnMouseLeave();
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnRButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	m_mouse->OnRightPressed(pt.x, pt.y);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnRButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	m_mouse->OnRightReleased(pt.x, pt.y);
	// release mouse if outside of window
	if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
	{
		ReleaseCapture();
		m_mouse->OnMouseLeave();
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT ContentWindow::OnResize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	m_deviceResources->OnResize();
	m_scene->WindowResized();
	return 0;
}

LRESULT ContentWindow::OnMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// in client region -> log move, and log enter + capture mouse (if not previously in window)
		if (pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height)
		{
			m_mouse->OnMouseMove(pt.x, pt.y);
			if (!m_mouse->IsInWindow()) // IsInWindow() will tell you if it was PREVIOUSLY in the window or not
			{
				SetCapture(hWnd);
				m_mouse->OnMouseEnter();
			}
		}
		// not in client -> log move / maintain capture if button down
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
			{
				m_mouse->OnMouseMove(pt.x, pt.y);
			}
			// button up -> release capture / log event for leaving
			else
			{
				ReleaseCapture();
				m_mouse->OnMouseLeave();
			}
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMouseLeave(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!m_io.WantCaptureMouse)
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		m_mouse->OnWheelDelta(pt.x, pt.y, delta);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void ContentWindow::DiscardGraphicsResources()
{

}

float ContentWindow::Height()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return static_cast<float>(rect.bottom);
}
float ContentWindow::Width()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	return static_cast<float>(rect.right);
}

LRESULT ContentWindow::OnGetMinMaxInfo(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT ContentWindow::OnChar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!m_io.WantCaptureKeyboard)
	{
		m_keyboard->OnChar(static_cast<unsigned char>(wParam));
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!m_io.WantCaptureKeyboard)
	{
		m_keyboard->OnKeyReleased(static_cast<unsigned char>(wParam));
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!m_io.WantCaptureKeyboard)
	{
		if (!(lParam & 0x40000000) || m_keyboard->AutorepeatIsEnabled()) // filter autorepeat
		{
			m_keyboard->OnKeyPressed(static_cast<unsigned char>(wParam));
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnSysKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return this->OnKeyUp(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnSysKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return this->OnKeyDown(hWnd, msg, wParam, lParam);
}

LRESULT ContentWindow::OnKillFocus(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// clear keystate when window loses focus to prevent input getting "stuck"
	m_keyboard->ClearState();
	return DefWindowProc(hWnd, msg, wParam, lParam);
}