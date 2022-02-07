#pragma once
#include "pch.h"
#include "ChameleonException.h"
#include "DxgiInfoManager.h"

#include <sstream>
#include <vector>

#define GFX_EXCEPT_NOINFO(hr) DeviceResourcesException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw DeviceResourcesException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) DeviceResourcesException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#else
#define GFX_EXCEPT(hr) DeviceResourcesException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException( __LINE__,__FILE__,(hr) )
#endif

class DeviceResourcesException : public ChameleonException
{
public:
	DeviceResourcesException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	HRESULT GetErrorCode() const noexcept;
	std::string GetErrorInfo() const noexcept;

	static std::string TranslateErrorCode(HRESULT hr) noexcept;

private:
	HRESULT hr;
	std::string m_info;
};

class DeviceRemovedException : public DeviceResourcesException
{
public:
	DeviceRemovedException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
	const char* GetType() const noexcept override;
private:
	std::string m_reason;
};