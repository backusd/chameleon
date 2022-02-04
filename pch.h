#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
//#define NOCTLMGR		--> Causes build to fail when #include <comdef.h> which is used for _com_error
#define NODRAWTEXT
#define NOKERNEL
//#define NONLS			--> Causes build to fail when #include <comdef.h> which is used for _com_error
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#define NOMINMAX

#define STRICT


#include <Windows.h>
#include <WinUser.h>


/* If you end up needed to include <wrl.h>, use the following:

#pragma warning(disable:4265)
#include <wrl.h>
#pragma warning(default:4265)

*/

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <d2d1_3.h>
#include <dwrite_3.h>
#pragma comment(lib, "Dwrite")
#include <wincodec.h>
#include <comdef.h>
#pragma comment(lib, "comsuppw")
#include <d3d11_4.h>
#pragma comment(lib, "D3D11")
#include <wrl/client.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler")