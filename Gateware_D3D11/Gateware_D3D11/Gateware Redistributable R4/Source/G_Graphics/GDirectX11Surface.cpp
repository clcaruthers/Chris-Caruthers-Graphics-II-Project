#include "../DLL_Export_Symbols.h"
#include "../../Interface/G_Graphics/GDirectX11Surface.h"
#include "../../Source/G_System/GUtility.h"



#ifdef _WIN32

#pragma comment (lib, "D3D11.lib")
#include <d3d11.h>
#include <atomic>
#include <mutex>
#include <thread>

#elif __linux__
#elif __APPLE__
#endif

using namespace GW;
using namespace CORE;
using namespace SYSTEM;
using namespace GRAPHICS;

class GDirectX11 : public GDirectX11Surface
{
private:
	// declare all necessary members
	unsigned int refCount;

	GWindow*					gWnd;
	HWND						surfaceWindow;
	ID3D11Device*				device;
	ID3D11DeviceContext*		context;
	IDXGISwapChain*				swapChain;
	ID3D11RenderTargetView*		rtv;
	ID3D11DepthStencilView*		zBuffer = nullptr;
	unsigned int				width;
	unsigned int				height;
	float						aspectRatio;

public:
	GDirectX11();
	virtual ~GDirectX11();
	void	SetGWindow(GWindow* _window);
	GReturn Initialize(unsigned long long _initMask);
	GReturn	GetAspectRatio(float& _outRatio);

	GReturn GetDevice(void** _outDevice);
	GReturn GetContext(void** _outContext);
	GReturn GetSwapchain(void** _outSwapchain);
	GReturn GetRenderTarget(void** _outRenderTarget);
	GReturn GetDepthStencilView(void** _outDepthStencilView);

	GReturn GetCount(unsigned int& _outCount);
	GReturn IncrementCount();
	GReturn DecrementCount();
	GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface);
	GReturn OnEvent(const GUUIID& _senderInerface, unsigned int _eventID, void* _eventData, unsigned int _dataSize);
};

GDirectX11::GDirectX11()
{
	ZeroMemory(&surfaceWindow, sizeof(HWND));
}

GDirectX11::~GDirectX11()
{
	if (device) device->Release();
	if (context) context->Release();
	if (rtv) rtv->Release();
	if (swapChain) swapChain->Release();
}

void GDirectX11::SetGWindow(GWindow* _window)
{
	gWnd = _window;
}

GReturn GDirectX11::Initialize(unsigned long long _initMask)
{
	gWnd->OpenWindow();
	gWnd->GetWindowHandle(sizeof(HWND), (void**)&surfaceWindow);
	RECT windowRect;
	GetWindowRect(surfaceWindow,&windowRect);
	gWnd->GetClientWidth(width);
	gWnd->GetClientHeight(height);
	aspectRatio = (float)width / (float)height;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	D3D11_CREATE_DEVICE_FLAG deviceFlag = D3D11_CREATE_DEVICE_SINGLETHREADED;

#ifdef _DEBUG
	deviceFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swapChainStruct;
	swapChainStruct.BufferCount = 1;

	if (_initMask & COLOR_10_BIT)
		swapChainStruct.BufferDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	else
		swapChainStruct.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	swapChainStruct.BufferDesc.Width = width;
	swapChainStruct.BufferDesc.Height = height;
	ZeroMemory(&swapChainStruct.BufferDesc.RefreshRate, sizeof(swapChainStruct.BufferDesc.RefreshRate));
	ZeroMemory(&swapChainStruct.BufferDesc.Scaling, sizeof(swapChainStruct.BufferDesc.Scaling));
	ZeroMemory(&swapChainStruct.BufferDesc.ScanlineOrdering, sizeof(swapChainStruct.BufferDesc.ScanlineOrdering));
	swapChainStruct.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainStruct.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainStruct.OutputWindow = surfaceWindow;
	swapChainStruct.Windowed = true;
	swapChainStruct.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainStruct.SampleDesc.Count = 1;
	swapChainStruct.SampleDesc.Quality = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		deviceFlag,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&swapChainStruct,
		&swapChain,
		&device,
		nullptr,
		&context
	);	

	ID3D11Resource* buffer;
	swapChain->GetBuffer(0, __uuidof(buffer), reinterpret_cast<void**>(&buffer));
	device->CreateRenderTargetView(buffer, NULL,&rtv);

	buffer->Release();

	if (_initMask & DEPTH_BUFFER_SUPPORT)
	{
		/////////////////////////////////
		// Create Depth Buffer Texture //
		/////////////////////////////////

		D3D11_TEXTURE2D_DESC depthTextureDesc = { 0 };
		depthTextureDesc.Width = width;
		depthTextureDesc.Height = height;
		depthTextureDesc.ArraySize = 1;
		depthTextureDesc.MipLevels = 1;
		depthTextureDesc.SampleDesc.Count = 1;

		if (_initMask & DEPTH_STENCIL_SUPPORT)
			depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		else
			depthTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;		
		
		depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ID3D11Texture2D* depthBuffer;
		device->CreateTexture2D(&depthTextureDesc, NULL,&depthBuffer);

		///////////////////////////////
		// Create Depth Stencil View //
		///////////////////////////////

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		if (_initMask & DEPTH_STENCIL_SUPPORT)
			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		else
			depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;

		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		hr = device->CreateDepthStencilView(depthBuffer, &depthStencilViewDesc, &zBuffer);
		depthBuffer->Release();
	}

	/////////////////////////
	// Initialize Viewport //
	/////////////////////////

	D3D11_VIEWPORT viewport;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	gWnd->GetClientTopLeft((unsigned int&)viewport.TopLeftX, (unsigned int&)viewport.TopLeftY);

	context->RSSetViewports(1, &viewport);

	return SUCCESS;
}

GReturn GDirectX11::GetDevice(void** _outDevice)
{
	if (device == nullptr)
		return FAILURE;

	*_outDevice = device;

	return SUCCESS;
}

GReturn GDirectX11::GetContext(void** _outContext)
{
	if (context == nullptr)
		return FAILURE;

	*_outContext = context;

	return SUCCESS;
}

GReturn GDirectX11::GetSwapchain(void** _outSwapchain)
{
	if (swapChain == nullptr)
		return FAILURE;

	*_outSwapchain = swapChain;

	return SUCCESS;
}

GReturn GDirectX11::GetRenderTarget(void** _outRenderTarget)
{
	if (rtv == nullptr)
		return FAILURE;

	*_outRenderTarget = rtv;

	return SUCCESS;
}

GReturn GDirectX11::GetDepthStencilView(void** _outDepthStencilView)
{
	if (zBuffer == nullptr)
		return FAILURE;

	*_outDepthStencilView = zBuffer;

	return SUCCESS;
}

GReturn GDirectX11::GetAspectRatio(float& _outRatio)
{
	if (!gWnd)
		return FAILURE;

	_outRatio = aspectRatio;

	return SUCCESS;
}

GReturn GDirectX11::GetCount(unsigned int& _outCount)
{
	_outCount = refCount;

	return SUCCESS;
}

GReturn GDirectX11::IncrementCount()
{
	if (refCount == G_UINT_MAX)
		return FAILURE;

	++refCount;

	return SUCCESS;
}

GReturn GDirectX11::DecrementCount()
{
	if (refCount == 0)
		return FAILURE;

	--refCount;

	if (refCount == 0)
	{
		gWnd->DeregisterListener(this);
		delete this;
	}


	return SUCCESS;
}

GReturn GDirectX11::RequestInterface(const GUUIID& _interfaceID, void** _outputInterface)
{
	if (_outputInterface == nullptr)
		return INVALID_ARGUMENT;

	if (_interfaceID == GWindowUUIID)
	{
		GWindow* convert = reinterpret_cast<GWindow*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GBroadcastingUUIID)
	{
		GBroadcasting* convert = reinterpret_cast<GBroadcasting*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GMultiThreadedUUIID)
	{
		GMultiThreaded* convert = reinterpret_cast<GMultiThreaded*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GInterfaceUUIID)
	{
		GInterface* convert = reinterpret_cast<GInterface*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GDirectX11SurfaceUUIID)
	{
		GDirectX11Surface* convert = reinterpret_cast<GDirectX11Surface*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else
		return INTERFACE_UNSUPPORTED;

	return SUCCESS;
}

GReturn GDirectX11::OnEvent(const GUUIID& _senderInerface, unsigned int _eventID, void * _eventData, unsigned int _dataSize)
{

	if (_senderInerface == GWindowUUIID)
	{

		GWINDOW_EVENT_DATA* eventStruct = (GWINDOW_EVENT_DATA*)_eventData;

		switch (_eventID)
		{
		case GW::SYSTEM::MAXIMIZE:
		case GW::SYSTEM::RESIZE:
		{

			gWnd->GetClientWidth(width);
			gWnd->GetClientHeight(height);

			aspectRatio = (float)width / (float)height;

			if (swapChain)
			{
				rtv->Release();

				HRESULT result = swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

				if (result != S_OK)
					return FAILURE;

				ID3D11Texture2D* newRTVBuffer;
				result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&newRTVBuffer));

				if (result != S_OK)
					return FAILURE;

				result = device->CreateRenderTargetView(newRTVBuffer, NULL,&rtv);

				if (result != S_OK)
					return FAILURE;

				D3D11_TEXTURE2D_DESC depthTextureDesc = { 0 };
				depthTextureDesc.Width = width;
				depthTextureDesc.Height = height;
				depthTextureDesc.ArraySize = 1;
				depthTextureDesc.MipLevels = 1;
				depthTextureDesc.SampleDesc.Count = 1;
				depthTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
				depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

				ID3D11Texture2D* depthBuffer;
				device->CreateTexture2D(&depthTextureDesc, NULL,&depthBuffer);

				D3D11_DEPTH_STENCIL_VIEW_DESC newDSVdesc;
				ZeroMemory(&newDSVdesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
				newDSVdesc.Format = DXGI_FORMAT_D32_FLOAT;
				newDSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
				device->CreateDepthStencilView(depthBuffer,&newDSVdesc,&zBuffer);

				newRTVBuffer->Release();

				D3D11_VIEWPORT viewport;
				viewport.TopLeftX = 0;
				viewport.TopLeftY = 0;
				viewport.Width = width;
				viewport.Height = height;
				viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;

				context->RSSetViewports(1,&viewport);
			}

		}
			break;

		case GW::SYSTEM::MOVE:
		{
			unsigned int newX;
			unsigned int newY;
			unsigned int currWidth;
			unsigned int currHeight;
			gWnd->GetClientWidth(currWidth);
			gWnd->GetClientHeight(currHeight);
			gWnd->GetClientTopLeft(newX, newY);

			D3D11_VIEWPORT viewport;
			ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
			viewport.Width = (float)currWidth;
			viewport.Height = (float)currHeight;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;

			context->RSSetViewports(1,&viewport);
		}
			break;
		case GW::SYSTEM::DESTROY:
		{
			this->~GDirectX11();
		}
			break;
		}

	}

	return SUCCESS;
}

GATEWARE_EXPORT_EXPLICIT GReturn CreateGDirectX11Surface(SYSTEM::GWindow* _gWin, unsigned long long _initMask, GDirectX11Surface** _outSurface)
{
	return GW::GRAPHICS::CreateGDirectX11Surface(_gWin, _initMask, _outSurface);
}

GReturn GW::GRAPHICS::CreateGDirectX11Surface(SYSTEM::GWindow* _gWin, unsigned long long _initMask, GDirectX11Surface** _outSurface)
{

	if (_outSurface == nullptr)
		return INVALID_ARGUMENT;

	GDirectX11* Surface = new GDirectX11();
	Surface->SetGWindow(_gWin);

	Surface->Initialize(_initMask);

	_gWin->RegisterListener(Surface, 0);

	if (Surface == nullptr)
		return FAILURE;

	*_outSurface = Surface;

	return SUCCESS;
}