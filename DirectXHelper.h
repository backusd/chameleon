#pragma once
#include "pch.h"

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw _com_error(hr);
	}
}

inline bool SdkLayersAvailable()
{
	return SUCCEEDED(
		D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,		// There is no need to create a real hardware device
			0,
			D3D11_CREATE_DEVICE_DEBUG,	// Check for the SDK Layers
			nullptr,					// Any feature level will do
			0,
			D3D11_SDK_VERSION,			// Always set this to this value for Windows Store Apps
			nullptr,					// No need to keep the D3D Device reference
			nullptr,					// No need to know the feature level
			nullptr						// No need to keeo the D3D Device Context Reference
		)
	);
}

