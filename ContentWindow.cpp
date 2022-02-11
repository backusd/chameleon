#include "ContentWindow.h"

using Microsoft::WRL::ComPtr;
using DirectX::XMFLOAT3;


ContentWindow::ContentWindow(int width, int height, const char* name) :
	WindowBase(width, height, name),
	// m_stateBlock(nullptr),
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

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();	

	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
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
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
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
	ObjectStoreAddMeshes();
	ObjectStoreAddConstantBuffers();
	ObjectStoreAddRasterStates();
	ObjectStoreAddSamplerStates();
	ObjectStoreAddTextures();
	ObjectStoreAddDepthStencilStates();

	// Create the state block 
	//HRESULT hr;
	//GFX_THROW_NOINFO(
	//	m_deviceResources->D2DFactory()->CreateDrawingStateBlock(m_stateBlock.GetAddressOf())
	//);

	m_timer = std::make_shared<StepTimer>();

	m_cpu = std::make_shared<CPU>(m_timer);

	m_keyboard = std::make_shared<Keyboard>();
	m_mouse = std::make_shared<Mouse>();

	// m_network = std::make_shared<Network>("155.248.215.180", 7000, m_timer);

	m_hud = std::make_shared<HUD>(m_deviceResources);
	m_scene = std::make_shared<Scene>(m_deviceResources, m_hWnd);	

}

ContentWindow::~ContentWindow()
{
	// Have to make sure to delete objects on close
	ObjectStore::DestructObjects();
}

void ContentWindow::ObjectStoreAddShaders()
{
	// Basic Cube ==================================================================================================
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	ObjectStore::AddVertexShaderAndInputLayout(L"VertexShader.cso", ied, static_cast<UINT>(std::size(ied)), "basic-cube-vertex-shader");
	ObjectStore::AddPixelShader(L"PixelShader.cso", "basic-cube-pixel-shader");

	// Terrain =====================================================================================================
	const D3D11_INPUT_ELEMENT_DESC terrainDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	ObjectStore::AddVertexShaderAndInputLayout(L"TerrainVertexShader.cso", terrainDesc, static_cast<UINT>(std::size(terrainDesc)), "terrain-vertex-shader");
	ObjectStore::AddPixelShader(L"TerrainPixelShader.cso", "terrain-pixel-shader");

	// Terrain Texture ==============================================================================================
	const D3D11_INPUT_ELEMENT_DESC terrainTextureDesc[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	ObjectStore::AddVertexShaderAndInputLayout(L"TerrainTextureVertexShader.cso", terrainTextureDesc, static_cast<UINT>(std::size(terrainTextureDesc)), "terrain-texture-vertex-shader");
	ObjectStore::AddPixelShader(L"TerrainTexturePixelShader.cso", "terrain-texture-pixel-shader");

	// Phong ======================================================================================================
	const D3D11_INPUT_ELEMENT_DESC phongVertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	ObjectStore::AddVertexShaderAndInputLayout(L"PhongVertexShader.cso", phongVertexDesc, static_cast<UINT>(std::size(phongVertexDesc)), "phong-vertex-shader");
	ObjectStore::AddPixelShader(L"PhongPixelShader.cso", "phong-pixel-shader");


	// Sky Dome ======================================================================================================
	const D3D11_INPUT_ELEMENT_DESC skyDomeVertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	ObjectStore::AddVertexShaderAndInputLayout(L"SkyDomeVertexShader.cso", skyDomeVertexDesc, static_cast<UINT>(std::size(skyDomeVertexDesc)), "sky-dome-vertex-shader");
	ObjectStore::AddPixelShader(L"SkyDomePixelShader.cso", "sky-dome-pixel-shader");



}
void ContentWindow::ObjectStoreAddMeshes()
{
	ObjectStore::AddMesh(std::make_shared<BoxMesh>(m_deviceResources), "box-mesh");
	ObjectStore::AddMesh(std::make_shared<TerrainMesh>(m_deviceResources), "terrain-mesh");
	ObjectStore::AddMesh(std::make_shared<SkyDomeMesh>(m_deviceResources), "sky-dome-mesh");
}
void ContentWindow::ObjectStoreAddConstantBuffers()
{
	ObjectStore::AddConstantBuffer<ModelViewProjectionConstantBuffer>("model-view-projection-buffer", D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	ObjectStore::AddConstantBuffer<PhongMaterialProperties>("phong-material-properties-buffer", D3D11_USAGE_DEFAULT, 0);
	ObjectStore::AddConstantBuffer<LightProperties>("light-properties-buffer", D3D11_USAGE_DEFAULT, 0);
	ObjectStore::AddConstantBuffer<TerrainMatrixBufferType>("terrain-constant-buffer", D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	ObjectStore::AddConstantBuffer<TerrainLightBufferType>("terrain-light-buffer", D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	ObjectStore::AddConstantBuffer<SkyDomeWorldViewProjectionBufferType>("sky-dome-constant-buffer", D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	ObjectStore::AddConstantBuffer<SkyDomeColorBufferType>("sky-dome-gradient-buffer", D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}
void ContentWindow::ObjectStoreAddRasterStates()
{
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID; // or D3D11_FILL_WIREFRAME
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = true;	// This must be true for the outline effect to work properly
	rd.DepthBias = 0;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;

	ObjectStore::AddRasterState(rd, "solidfill");

	rd.FillMode = D3D11_FILL_WIREFRAME;

	ObjectStore::AddRasterState(rd, "wireframe");
}
void ContentWindow::ObjectStoreAddSamplerStates()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ObjectStore::AddSamplerState(samplerDesc, "terrain-texture-sampler");
}
void ContentWindow::ObjectStoreAddTextures()
{
	//std::string filename = "test.tga";
	std::string filename = "dirt01d.tga";
	ObjectStore::AddTexture(std::make_shared<Texture>(m_deviceResources, filename), "terrain-texture");
	ObjectStore::AddTexture(std::make_shared<Texture>(m_deviceResources, "dirt01n.tga"), "terrain-normal-map-texture");
}
void ContentWindow::ObjectStoreAddDepthStencilStates()
{
	ObjectStore::AddDepthStencilState(std::make_shared<DepthStencilState>(m_deviceResources, true), "depth-enabled-depth-stencil-state");
	ObjectStore::AddDepthStencilState(std::make_shared<DepthStencilState>(m_deviceResources, false), "depth-disabled-depth-stencil-state");
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
	// Don't try to render anything before the first Update.
	//if (m_timer->GetFrameCount() == 0)
	//	return false;

	ID3D11DeviceContext4* context = m_deviceResources->D3DDeviceContext();

	m_deviceResources->ResetViewport();

	FLOAT background[4] = { 45.0f / 255.0f, 45.0f / 255.0f, 48.0f / 255.0f };
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), background);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());


	 
	// Draw all 3D simulation controls first
	//m_layout->Render3DControls();

	
	m_scene->Draw();



	// Draw all 2D / Menu controls next
	m_deviceResources->ResetViewport();

	/*
	ID2D1DeviceContext* context2 = m_deviceResources->D2DDeviceContext();
	context2->SaveDrawingState(m_stateBlock.Get());
	context2->BeginDraw();
	context2->SetTransform(m_deviceResources->OrientationTransform2D());

	//m_layout->Render2DControls();

	// re-render the captured control to make sure it is on top of the UI
	//m_layout->Render2DCapturedControl();
	m_hud->Draw();


	HRESULT hr = context2->EndDraw();
	if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
	{
		DiscardGraphicsResources();
	}

	context2->RestoreDrawingState(m_stateBlock.Get());
	*/

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (m_show_demo_window)
		ImGui::ShowDemoWindow(&m_show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &m_show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &m_show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&m_clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (m_show_another_window)
	{
		ImGui::Begin("Another Window", &m_show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			m_show_another_window = false;
		ImGui::End();
	}


	// Render the Scene







	// Rendering
	ImGui::Render();
	//const float clear_color_with_alpha[4] = { m_clear_color.x * m_clear_color.w, m_clear_color.y * m_clear_color.w, m_clear_color.z * m_clear_color.w, m_clear_color.w };
	//g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	//context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), clear_color_with_alpha);
	
	
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
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
	//m_mouse->OnLeftPressed(pt.x, pt.y);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnLButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	//m_mouse->OnLeftReleased(pt.x, pt.y);
	// release mouse if outside of window
	if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
	{
		ReleaseCapture();
		//m_mouse->OnMouseLeave();
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnLButtonDoubleClick(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	//m_mouse->OnLeftDoubleClick(pt.x, pt.y);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	//m_mouse->OnMiddlePressed(pt.x, pt.y);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	//m_mouse->OnMiddleReleased(pt.x, pt.y);
	// release mouse if outside of window
	if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
	{
		ReleaseCapture();
		//m_mouse->OnMouseLeave();
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnRButtonDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	//m_mouse->OnRightPressed(pt.x, pt.y);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnRButtonUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	//m_mouse->OnRightReleased(pt.x, pt.y);
	// release mouse if outside of window
	if (pt.x < 0 || pt.x >= m_width || pt.y < 0 || pt.y >= m_height)
	{
		ReleaseCapture();
		//m_mouse->OnMouseLeave();
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT ContentWindow::OnResize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return 0;
}

LRESULT ContentWindow::OnMouseMove(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	/*
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
	}*/

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMouseLeave(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnMouseWheel(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	const POINTS pt = MAKEPOINTS(lParam);
	const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
	//m_mouse->OnWheelDelta(pt.x, pt.y, delta);

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
	//m_keyboard->OnChar(static_cast<unsigned char>(wParam));
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnKeyUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//m_keyboard->OnKeyReleased(static_cast<unsigned char>(wParam));
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT ContentWindow::OnKeyDown(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//if (!(lParam & 0x40000000) || m_keyboard->AutorepeatIsEnabled()) // filter autorepeat
	//{
	//	m_keyboard->OnKeyPressed(static_cast<unsigned char>(wParam));
	//}
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
	//m_keyboard->ClearState();
	return DefWindowProc(hWnd, msg, wParam, lParam);
}