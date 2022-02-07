#pragma once
#include "pch.h"
#include "DirectXHelper.h"
#include "DxgiInfoManager.h"
#include "DeviceResourcesException.h"





class DeviceResources
{
public:
	DeviceResources(HWND hWnd);
	DeviceResources(const DeviceResources&) = delete;
	DeviceResources& operator=(const DeviceResources&) = delete;
	~DeviceResources();

	// May need to expand this method...
	void OnResize() { CreateWindowSizeDependentResources(); }

	// Direct 2D drawing components
	ID2D1Factory7* D2DFactory() const { return m_d2dFactory.Get(); }
	ID2D1Device6* D2DDevice() const { return m_d2dDevice.Get(); }
	ID2D1DeviceContext6* D2DDeviceContext() const { return m_d2dDeviceContext.Get(); }
	ID2D1Bitmap1* D2DBitmap() const { return m_d2dBitmap.Get(); }

	D2D1::Matrix3x2F OrientationTransform2D() const { return m_orientationTransform2D; }

	// Direct Write drawing components
	IDWriteFactory7* DWriteFactory() const { return m_dwriteFactory.Get(); }
	IWICImagingFactory2* WICImagingFactory() const { return m_wicImagingFactory.Get(); }

	// Direct3D objects
	ID3D11Device5* D3DDevice() const { return m_d3dDevice.Get(); }
	ID3D11DeviceContext4* D3DDeviceContext() const { return m_d3dDeviceContext.Get(); }

	D3D_FEATURE_LEVEL D3DFeatureLevel() { return m_d3dFeatureLevel; }

	DirectX::XMFLOAT4X4 OrientationTransform3D() const { return m_orientationTransform3D; }


	D3D11_VIEWPORT GetScreenViewport() const { return m_viewport; }
	void SetViewport(CD3D11_VIEWPORT viewport);
	void ResetViewport();

	ID3D11RenderTargetView1* GetBackBufferRenderTargetView() const { return m_d3dRenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_d3dDepthStencilView.Get(); }
	void Present();

	// DPI Scaling functions
	float DIPSToPixels(float dips) { return dips * m_dpiScale; }
	float PixelsToDIPS(float pixels) { return pixels / m_dpiScale; }

	RECT WindowRect();


private:

	void CreateDeviceDependentResources();
	void CreateDeviceIndependentResources();
	void CreateWindowSizeDependentResources();
	void HandleDeviceLost();

#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif

	// Keep handle to window
	HWND m_hWnd;

	// DPI scale for the window
	float m_dpiScale;

	// ==========================================================

	// Direct 2D drawing components
	Microsoft::WRL::ComPtr<ID2D1Factory7>		m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device6>		m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext6>	m_d2dDeviceContext;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>		m_d2dBitmap;

	// Direct Write drawing components
	Microsoft::WRL::ComPtr<IDWriteFactory7>		m_dwriteFactory;
	Microsoft::WRL::ComPtr<IWICImagingFactory2>	m_wicImagingFactory;

	// Direct3D objects
	Microsoft::WRL::ComPtr<ID3D11Device5>		 m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext4> m_d3dDeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain4>		 m_dxgiSwapChain;

	// Direct3D Rendering objects ------ THESE MAY END UP GETTING STORED PER WINDOW
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView1>	m_d3dRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_d3dDepthStencilView;

	D3D11_VIEWPORT m_viewport;

	// Cached device properties
	D3D_FEATURE_LEVEL m_d3dFeatureLevel;

	// Orientation transforms
	D2D1::Matrix3x2F	m_orientationTransform2D;
	DirectX::XMFLOAT4X4 m_orientationTransform3D;

};