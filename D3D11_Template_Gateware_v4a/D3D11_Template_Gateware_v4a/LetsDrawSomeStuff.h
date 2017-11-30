// This file can be used to contain very basic DX11 Loading, Drawing & Clean Up. (Start Here, or Make your own set of classes)
#pragma once
// Include our DX11 middle ware
#include "Gateware Redistributable R4a/Interface/G_Graphics/GDirectX11Surface.h"
// You can clean this up and make it much nicer, otherwise you are stuck in debug 32bit :P
#pragma comment(lib, "Gateware Redistributable R4a/Archive/Win32/Gateware_i386/Debug/GDirectX11Surface.lib")

// Include DirectX11 for interface access
#include <d3d11.h>
#include <DirectXMath.h>
#include "XTime.h"
#include "DDSTextureLoader.h"
#include "TentacleKnight.h"
#include "Wolf.h"

#include "Trivial_PS.csh"
#include "Trivial_VS.csh"

using namespace DirectX;

// Simple Container class to make life easier/cleaner
class LetsDrawSomeStuff
{
	// variables here
	GW::GRAPHICS::GDirectX11Surface* mySurface = nullptr;
	// Gettting these handles from GDirectX11Surface will increase their internal refrence counts, be sure to "Release()" them when done!
	ID3D11Device *myDevice = nullptr;
	IDXGISwapChain *mySwapChain = nullptr;
	ID3D11DeviceContext *myContext = nullptr;

	// TODO: Add your own D3D11 variables here (be sure to "Release()" them when done!)
	D3D11_VIEWPORT viewport;

	ID3D11Buffer * vertBuffer;
	unsigned int vertCount;
	ID3D11InputLayout * IL;
	ID3D11Buffer * vertBuffer2;
	ID3D11Buffer * lightConstBuff;
	ID3D11Buffer * pLightConBuff;

	ID3D11Buffer * sprlVBuff;
	unsigned int sprlVCount;

	/*ID3D11Buffer * TKvertBuffer;
	ID3D11Buffer * TKIndexBuffer;
	unsigned int TKvertCount;*/

	ID3D11Buffer * WolfVertBuffer;
	ID3D11Buffer * WolfIndexBuffer;
	unsigned int WolfVertCount;


	ID3D11VertexShader * vShader;
	ID3D11PixelShader * pShader;

	XTime timeObject;

	XMFLOAT4X4 VIEWMATRIX;
	XMFLOAT4X4 PROJECTIONMATRIX;
	
	XMFLOAT4X4 WORLDMATRIX;
	/*XMFLOAT4X4 TKWORLD;*/
	XMFLOAT4X4 WOLFWORLD;
	XMFLOAT4X4 SPRLWORLD;

	float xRot = 0;
	float yRot = 0;
	float xShift = 0;
	float yShift = 0;
	float zShift = 0;

	float FOV = 90;
	float aspectRatio;

	float zFar = 10.0f;
	float zNear = 0.1f;

	ID3D11Texture2D * floorTex;
	ID3D11ShaderResourceView * floorSRV;
	ID3D11SamplerState * texSampler;

	//ID3D11Texture2D * TKTex;
	//ID3D11ShaderResourceView * TKSRV;
	
	ID3D11Texture2D * wolfTex;
	ID3D11ShaderResourceView * wolfSRV;

	struct DIRLIGHT_TO_PSHADER {
		XMFLOAT4 light;
		XMFLOAT4 color;
	};

	struct POINTLIGHT_TO_PSHADER {
		XMFLOAT4 lightPos;
		XMFLOAT4 color;
		float radius;
		XMFLOAT3 pad;
	};

	struct SEND_TO_VRAM {
		XMFLOAT4X4 worldMat;
		XMFLOAT4X4 viewMat;
		XMFLOAT4X4 projMat;
	};

	SEND_TO_VRAM toShader;
	DIRLIGHT_TO_PSHADER dirLight;
	POINTLIGHT_TO_PSHADER pLight;
	bool PLGrow = true;

public:

	struct MYVERTEX {
		XMFLOAT4 XYZW;
		XMFLOAT4 UV;
		XMFLOAT4 NORM;
		XMFLOAT4 RGBA;
	};

	// Init
	LetsDrawSomeStuff(GW::SYSTEM::GWindow* attatchPoint);
	// Shutdown
	~LetsDrawSomeStuff();
	// Draw
	void Render();
};

// Init
LetsDrawSomeStuff::LetsDrawSomeStuff(GW::SYSTEM::GWindow* attatchPoint)
{
	if (attatchPoint) // valid window?
	{
		// Create surface, will auto attatch to GWindow
		if (G_SUCCESS(GW::GRAPHICS::CreateGDirectX11Surface(attatchPoint, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT, &mySurface)))
		{
			// Grab handles to all DX11 base interfaces
			mySurface->GetDevice((void**)&myDevice);
			mySurface->GetSwapchain((void**)&mySwapChain);
			mySurface->GetContext((void**)&myContext);

			UINT wWidth;
			UINT wHeight;
			UINT wSize;
			void * wWindow;

			attatchPoint->GetWidth(wWidth);
			attatchPoint->GetHeight(wHeight);
			attatchPoint->GetWindowHandle(sizeof(HWND), &wWindow);

			// TODO: Create new DirectX stuff here! (Buffers, Shaders, Layouts, Views, Textures, etc...)
			/*viewport.MinDepth = 0;
			viewport.MaxDepth = 1;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Height = (FLOAT)wHeight;
			viewport.Width = (FLOAT)wWidth;*/

			sprlVCount = 10000;
			MYVERTEX * spiralVerts = new MYVERTEX[10000];

			for (int i = 0; i < sprlVCount; ++i) {
				float xo = (float)(rand() % 100) / 4500.0f;
				float yo = (float)(rand() % 100) / 4500.0f;
				float zo = (float)(rand() % 100) / 4500.0f;
				spiralVerts[i].XYZW.x = ((cos(XMConvertToRadians(i)) * ((float)i / (float)sprlVCount)) / 2) + xo;
				spiralVerts[i].XYZW.y = ((sin(XMConvertToRadians(i)) * ((float)i / (float)sprlVCount)) / 2) + yo;
				spiralVerts[i].XYZW.z = ((float)i / (float)sprlVCount) + zo;
				spiralVerts[i].XYZW.w = 1;
				spiralVerts[i].UV = { 0, 0, 0, 0 };
				spiralVerts[i].NORM = { 1, 0, 0, 0 };
				spiralVerts[i].RGBA = { 1, 0, 1, 1 };
			}

			// TODO: PART 2 STEP 3a
			vertCount = 6;
			MYVERTEX verts[6];

#pragma region
			verts[0].XYZW.x = -1;
			verts[0].XYZW.y = 0;
			verts[0].XYZW.z = 1;
			verts[0].XYZW.w = 1;
			verts[0].UV.x = 0;
			verts[0].UV.y = 0;
			verts[0].UV.z = 0;
			verts[0].UV.w = 0;
			verts[0].NORM.x = 0;
			verts[0].NORM.y = 1;
			verts[0].NORM.z = 0;
			verts[0].NORM.w = 0;
			verts[0].RGBA.x = 1;
			verts[0].RGBA.y = 1;
			verts[0].RGBA.z = 1;
			verts[0].RGBA.w = 1;

			verts[1].XYZW.x = 1;
			verts[1].XYZW.y = 0;
			verts[1].XYZW.z = 1;
			verts[1].XYZW.w = 1;
			verts[1].UV.x = 1;
			verts[1].UV.y = 0;
			verts[1].UV.z = 0;
			verts[1].UV.w = 0;
			verts[1].NORM.x = 0;
			verts[1].NORM.y = 1;
			verts[1].NORM.z = 0;
			verts[1].NORM.w = 0;
			verts[1].RGBA.x = 1;
			verts[1].RGBA.y = 1;
			verts[1].RGBA.z = 1;
			verts[1].RGBA.w = 1;

			verts[2].XYZW.x = -1;
			verts[2].XYZW.y = 0;
			verts[2].XYZW.z = -1;
			verts[2].XYZW.w = 1;
			verts[2].UV.x = 0;
			verts[2].UV.y = 1;
			verts[2].UV.z = 0;
			verts[2].UV.w = 0;
			verts[2].NORM.x = 0;
			verts[2].NORM.y = 1;
			verts[2].NORM.z = 0;
			verts[2].NORM.w = 0;
			verts[2].RGBA.x = 1;
			verts[2].RGBA.y = 1;
			verts[2].RGBA.z = 1;
			verts[2].RGBA.w = 1;

			verts[3].XYZW.x = 1;
			verts[3].XYZW.y = 0;
			verts[3].XYZW.z = 1;
			verts[3].XYZW.w = 1;
			verts[3].UV.x = 1;
			verts[3].UV.y = 0;
			verts[3].UV.z = 0;
			verts[3].UV.w = 0;
			verts[3].NORM.x = 0;
			verts[3].NORM.y = 1;
			verts[3].NORM.z = 0;
			verts[3].NORM.w = 0;
			verts[3].RGBA.x = 1;
			verts[3].RGBA.y = 1;
			verts[3].RGBA.z = 1;
			verts[3].RGBA.w = 1;

			verts[4].XYZW.x = 1;
			verts[4].XYZW.y = 0;
			verts[4].XYZW.z = -1;
			verts[4].XYZW.w = 1;
			verts[4].UV.x = 1;
			verts[4].UV.y = 1;
			verts[4].UV.z = 0;
			verts[4].UV.w = 0;
			verts[4].NORM.x = 0;
			verts[4].NORM.y = 1;
			verts[4].NORM.z = 0;
			verts[4].NORM.w = 0;
			verts[4].RGBA.x = 1;
			verts[4].RGBA.y = 1;
			verts[4].RGBA.z = 1;
			verts[4].RGBA.w = 1;

			verts[5].XYZW.x = -1;
			verts[5].XYZW.y = 0;
			verts[5].XYZW.z = -1;
			verts[5].XYZW.w = 1;
			verts[5].UV.x = 0;
			verts[5].UV.y = 1;
			verts[5].UV.z = 0;
			verts[5].UV.w = 0;
			verts[5].NORM.x = 0;
			verts[5].NORM.y = 1;
			verts[5].NORM.z = 0;
			verts[5].NORM.w = 0;
			verts[5].RGBA.x = 1;
			verts[5].RGBA.y = 1;
			verts[5].RGBA.z = 1;
			verts[5].RGBA.w = 1;
#pragma endregion

//			TKvertCount = 20163;
//			MYVERTEX* TKverts = new MYVERTEX[20163];
//			unsigned int TKIcount = 20163;
//			unsigned int TKIndicies[20163];
//
//#pragma region
//			for (int i = 0; i < 19259; ++i) {
//				TKverts[i].XYZW.x = TentacleKnight_data[i].pos[0];
//				TKverts[i].XYZW.y = TentacleKnight_data[i].pos[1];
//				TKverts[i].XYZW.z = TentacleKnight_data[i].pos[2];
//				TKverts[i].XYZW.w = 1;
//				TKverts[i].UV.x = TentacleKnight_data[i].uvw[0];
//				TKverts[i].UV.y = TentacleKnight_data[i].uvw[1];
//				TKverts[i].UV.z = TentacleKnight_data[i].uvw[2];
//				TKverts[i].UV.w = 0;
//				TKverts[i].NORM.x = TentacleKnight_data[i].nrm[0];
//				TKverts[i].NORM.y = TentacleKnight_data[i].nrm[1];
//				TKverts[i].NORM.z = TentacleKnight_data[i].nrm[2];
//				TKverts[i].NORM.w = 0;
//				TKverts[i].RGBA.x = 1.0f;
//				TKverts[i].RGBA.y = 1.0f;
//				TKverts[i].RGBA.z = 1.0f;
//				TKverts[i].RGBA.w = 1.0f;
//			}
//
//			for (int i = 0; i < 20163; i++) {
//				TKIndicies[i] = TentacleKnight_indicies[i];
//			}
//#pragma endregion

			WolfVertCount = 1981;
			MYVERTEX* wolfVerts = new MYVERTEX[1981];
			unsigned int wolfICount = 6114;
			unsigned int wolfIndicies[6114];

#pragma region
			for (int i = 0; i < 1981; ++i) {
				wolfVerts[i].XYZW = { Wolf_data[i].pos[0], Wolf_data[i].pos[1], Wolf_data[i].pos[2], 1 };
				wolfVerts[i].UV = { Wolf_data[i].uvw[0], Wolf_data[i].uvw[1], Wolf_data[i].uvw[2], 0 };
				wolfVerts[i].NORM = { Wolf_data[i].nrm[0], Wolf_data[i].nrm[1], Wolf_data[i].nrm[2], 1 };
				wolfVerts[i].RGBA = { 1.0f, 1.0f, 1.0f, 1.0f };
			}

			for (int i = 0; i < 6114; ++i) {
				wolfIndicies[i] = Wolf_indicies[i];
			}
#pragma endregion

			CreateDDSTextureFromFile(myDevice, L"MetalFLoor.dds", (ID3D11Resource**)&floorTex, &floorSRV);
			/*CreateDDSTextureFromFile(myDevice, L"Diffuse_Knight_Cleansed.dds", (ID3D11Resource**)&TKTex, &TKSRV);*/
			CreateDDSTextureFromFile(myDevice, L"alphaBlackR.dds", (ID3D11Resource**)&wolfTex, &wolfSRV);
			// TODO: PART 2 STEP 3b
			D3D11_BUFFER_DESC bDesc;
			bDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.CPUAccessFlags = NULL;
			bDesc.ByteWidth = sizeof(MYVERTEX) * vertCount;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			// TODO: PART 2 STEP 3c
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = &verts;
			initData.SysMemPitch = 0;
			initData.SysMemSlicePitch = 0;
			// TODO: PART 2 STEP 3d
			myDevice->CreateBuffer(&bDesc, &initData, &vertBuffer);

			//spiral
			bDesc.ByteWidth = sizeof(MYVERTEX) * sprlVCount;
			D3D11_SUBRESOURCE_DATA sprlInit;
			sprlInit.pSysMem = spiralVerts;
			sprlInit.SysMemPitch = 0;
			sprlInit.SysMemSlicePitch = 0;

			myDevice->CreateBuffer(&bDesc, &sprlInit, &sprlVBuff);

			//TENTACLE KNIGHT BUFFER CREATION
			/*D3D11_BUFFER_DESC TKvertDesc;
			TKvertDesc.Usage = D3D11_USAGE_IMMUTABLE;
			TKvertDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			TKvertDesc.CPUAccessFlags = NULL;
			TKvertDesc.ByteWidth = sizeof(MYVERTEX) * TKvertCount;
			TKvertDesc.MiscFlags = 0;
			TKvertDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA TKinit;
			TKinit.pSysMem = TKverts;
			TKinit.SysMemPitch = 0;
			TKinit.SysMemSlicePitch = 0;

			myDevice->CreateBuffer(&TKvertDesc, &TKinit, &TKvertBuffer);*/

			D3D11_BUFFER_DESC TKIDesc;
			TKIDesc.Usage = D3D11_USAGE_IMMUTABLE;
			TKIDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
			TKIDesc.CPUAccessFlags = NULL;
			/*TKIDesc.ByteWidth = sizeof(unsigned int) * TKIcount;*/
			TKIDesc.MiscFlags = 0;
			TKIDesc.StructureByteStride = 0;

			/*TKinit.pSysMem = &TKIndicies;

			myDevice->CreateBuffer(&TKIDesc, &TKinit, &TKIndexBuffer);*/

			//wolf buffers
			bDesc.ByteWidth = sizeof(MYVERTEX) * WolfVertCount;
			
			D3D11_SUBRESOURCE_DATA wolfInit;
			wolfInit.pSysMem = wolfVerts;
			wolfInit.SysMemPitch = 0;
			wolfInit.SysMemSlicePitch = 0;

			myDevice->CreateBuffer(&bDesc, &wolfInit, &WolfVertBuffer);

			TKIDesc.ByteWidth = sizeof(unsigned int) * wolfICount;

			wolfInit.pSysMem = &wolfIndicies;

			myDevice->CreateBuffer(&TKIDesc, &wolfInit, &WolfIndexBuffer);
			

			// TODO: PART 2 STEP 7
			myDevice->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &vShader);
			myDevice->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pShader);
			// TODO: PART 2 STEP 8a

			D3D11_INPUT_ELEMENT_DESC IED[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
				D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT , 0, 16,
				D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT , 0, 32,
				D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48,
				D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			// TODO: PART 2 STEP 8b
			myDevice->CreateInputLayout(IED, 4, Trivial_VS, sizeof(Trivial_VS), &IL);

			// TODO: PART 3 STEP 3
			D3D11_BUFFER_DESC bDesc2;
			bDesc2.Usage = D3D11_USAGE_DYNAMIC;
			bDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bDesc2.MiscFlags = 0;
			bDesc2.StructureByteStride = 0;
			bDesc2.ByteWidth = sizeof(SEND_TO_VRAM);

			D3D11_SAMPLER_DESC tsDesc;
			ZeroMemory(&tsDesc, sizeof(tsDesc));
			tsDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			tsDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			tsDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			tsDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			tsDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			tsDesc.MinLOD = 0;
			tsDesc.MaxLOD = D3D11_FLOAT32_MAX;
			myDevice->CreateSamplerState(&tsDesc, &texSampler);

			myDevice->CreateBuffer(&bDesc2, NULL, &vertBuffer2);

			bDesc2.ByteWidth = sizeof(DIRLIGHT_TO_PSHADER);

			myDevice->CreateBuffer(&bDesc2, NULL, &lightConstBuff);

			bDesc2.ByteWidth = sizeof(POINTLIGHT_TO_PSHADER);

			myDevice->CreateBuffer(&bDesc2, NULL, &pLightConBuff);

			//WORLD MATRICES CREATION
			XMStoreFloat4x4(&WORLDMATRIX, XMMatrixIdentity());
			/*XMStoreFloat4x4(&TKWORLD, XMMatrixTranslation(0, 0.5f, 0) * XMMatrixScaling(0.5f, 0.5f, 0.5f));*/
			XMStoreFloat4x4(&WOLFWORLD, XMMatrixTranslation(0.5f, 0, 0));
			XMStoreFloat4x4(&SPRLWORLD, XMMatrixTranslation(0.45f, 0.68f, 0.8f));

			pLight.color = { 1, 1, 0, 1 };
			pLight.lightPos = { 0, 0.4f, 0, 1 };
			pLight.radius = 0.5f;

			dirLight.light = { -1.0f, -1.0f, 0, 0 };
			dirLight.color = { 0.5f, 0.5f, 0.5f, 1.0f };

			//memory cleanup
			/*delete[] TKverts;*/
			delete[] wolfVerts;
			delete[] spiralVerts;
		}
	}
}

// Shutdown
LetsDrawSomeStuff::~LetsDrawSomeStuff()
{
	// Release DX Objects aquired from the surface
	myDevice->Release();
	mySwapChain->Release();
	myContext->Release();

	// TODO: "Release()" more stuff here!
	vertBuffer->Release();
	/*TKvertBuffer->Release();
	TKIndexBuffer->Release();*/
	WolfVertBuffer->Release();
	WolfIndexBuffer->Release();

	sprlVBuff->Release();

	vertBuffer2->Release();
	pShader->Release();
	vShader->Release();
	IL->Release();

	floorTex->Release();
	texSampler->Release();
	floorSRV->Release();

	/*TKTex->Release();
	TKSRV->Release();*/

	lightConstBuff->Release();
	pLightConBuff->Release();

	wolfTex->Release();
	wolfSRV->Release();


	if (mySurface) // Free Gateware Interface
	{
		mySurface->DecrementCount(); // reduce internal count (will auto delete on Zero)
		mySurface = nullptr; // the safest way to fly
	}
}

// Draw
void LetsDrawSomeStuff::Render()
{
	if (mySurface) // valid?
	{
		// this could be changed during resolution edits, get it every frame
		ID3D11RenderTargetView *myRenderTargetView = nullptr;
		ID3D11DepthStencilView *myDepthStencilView = nullptr;
		if (G_SUCCESS(mySurface->GetRenderTarget((void**)&myRenderTargetView)))
		{
			// Grab the Z Buffer if one was requested
			if (G_SUCCESS(mySurface->GetDepthStencilView((void**)&myDepthStencilView)))
			{
				myContext->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0); // clear it to Z exponential Far.
				myDepthStencilView->Release();
			}

			mySurface->GetAspectRatio(aspectRatio);
			XMStoreFloat4x4(&PROJECTIONMATRIX, XMMatrixPerspectiveFovLH(XMConvertToRadians(FOV), aspectRatio, zNear, zFar));

			timeObject.Signal();
			double timestep = timeObject.Delta();

			if (GetAsyncKeyState(VK_DOWN)) {
				xRot += 50 * timestep;
			}
			else if (GetAsyncKeyState(VK_UP)) {
				xRot -= 50 * timestep;
			}

			if (GetAsyncKeyState(VK_RIGHT)) {
				yRot += 50 * timestep;
			}
			else if (GetAsyncKeyState(VK_LEFT)) {
				yRot -= 50 * timestep;
			}

			if (GetAsyncKeyState(VK_SPACE))
				yShift += 0.5f * timestep;
			if (GetAsyncKeyState(VK_LCONTROL))
				yShift -= 0.5f * timestep;
			if (GetAsyncKeyState('W'))
				zShift += 0.5f * timestep;
			if (GetAsyncKeyState('S'))
				zShift -= 0.5f * timestep;
			if (GetAsyncKeyState('A'))
				xShift -= 0.5f * timestep;
			if (GetAsyncKeyState('D'))
				xShift += 0.5f * timestep;

			if (GetAsyncKeyState('Q'))
				FOV -= 10 * timestep;
			if (GetAsyncKeyState('E'))
				FOV += 10 * timestep;

			if (GetAsyncKeyState(VK_NUMPAD9))
				zFar += 10 * timestep;
			if (GetAsyncKeyState(VK_NUMPAD6))
				if (!((zFar - zNear) <= 1)) {
					zFar -= 10 * timestep;
				}
			if (GetAsyncKeyState(VK_NUMPAD8))
				if (!((zFar - zNear) <= 1)) {
					zNear += 10 * timestep;
				}
			if (GetAsyncKeyState(VK_NUMPAD5))
				zNear -= 10 * timestep;


			if (zFar < 1)
				zFar = 1;
			if (zNear < 0.1f)
				zNear = 0.1f;
			if (zNear >= zFar)
				zNear = zFar - 0.9f;

			if (FOV < 10)
				FOV = 10;
			else if (FOV > 179)
				FOV = 179;

			if (yRot >= 360)
				yRot -= 360;

			if (yRot < 0)
				yRot += 360;

			if (xRot >= 360)
				xRot -= 360;

			if (xRot < 0)
				xRot += 360;
			
			XMMATRIX xr = XMMatrixRotationX(XMConvertToRadians(xRot));
			XMMATRIX yr = XMMatrixRotationY(XMConvertToRadians(yRot));
			XMMATRIX rot = xr * yr;

			XMMATRIX translate = XMMatrixTranslation(xShift, yShift, zShift);

			XMMATRIX tempView = translate * rot;
			tempView = XMMatrixInverse(nullptr, tempView);
			XMStoreFloat4x4(&VIEWMATRIX, tempView);
			// Set active target for drawing, all array based D3D11 functions should use a syntax similar to below
			
			ID3D11RenderTargetView* const targets[] = { myRenderTargetView };
			myContext->OMSetRenderTargets(1, targets, myDepthStencilView);

			// Clear the screen to dark green
			const float d_green[] = { 0, 0.5f, 0, 1 };
			myContext->ClearRenderTargetView(myRenderTargetView, d_green);
			
			// TODO: Set your shaders, Update & Set your constant buffers, Attatch your vertex & index buffers, Set your InputLayout & Topology & Draw!

			toShader.worldMat = WORLDMATRIX;
			toShader.viewMat = VIEWMATRIX;
			toShader.projMat = PROJECTIONMATRIX;


			if (PLGrow) {
				pLight.radius += 0.5f * timestep;
			}
			else {
				pLight.radius -= 0.5f * timestep;
			}
			
			if (pLight.radius >= 2.0f) {
				PLGrow = false;
			}
			else if (pLight.radius < 0.1f) {
				PLGrow = true;
			}

			//set texture SRVs
			ID3D11ShaderResourceView * SRVs[] = { floorSRV, wolfSRV };

			//myContext->RSSetViewports(1, &viewport);


			D3D11_MAPPED_SUBRESOURCE mapResource;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			// TODO: PART 3 STEP 6
			myContext->VSSetConstantBuffers(0, 1, &vertBuffer2);

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(lightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &dirLight, sizeof(dirLight));
			myContext->Unmap(lightConstBuff, 0);

			myContext->PSSetConstantBuffers(0, 1, &lightConstBuff);

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(pLightConBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &pLight, sizeof(pLight));
			myContext->Unmap(pLightConBuff, 0);

			myContext->PSSetConstantBuffers(1, 1, &pLightConBuff);

			// TODO: PART 2 STEP 9a
			UINT stride = sizeof(MYVERTEX);
			UINT of = 0;
			myContext->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &of);
			// TODO: PART 2 STEP 9b
			myContext->VSSetShader(vShader, NULL, 0);
			myContext->PSSetShader(pShader, NULL, 0);
			myContext->PSSetShaderResources(0, 1, &SRVs[0]);
			myContext->PSSetSamplers(0, 1, &texSampler);

			// TODO: PART 2 STEP 9c
			myContext->IASetInputLayout(IL);

			// TODO: PART 2 STEP 9d
			myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			// TODO: PART 2 STEP 10
			myContext->Draw(6, 0);

			/*toShader.worldMat = TKWORLD;

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &TKvertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(TKIndexBuffer, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

			myContext->PSSetShaderResources(0, 1, &SRVs[1]);

			myContext->DrawIndexed(20163, 0, 0);*/

			//draw wolf
			toShader.worldMat = WOLFWORLD;

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &WolfVertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(WolfIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			myContext->PSSetShaderResources(0, 1, &SRVs[1]);

			myContext->DrawIndexed(6114, 0, 0);

			toShader.worldMat = SPRLWORLD;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &sprlVBuff, &stride, &of);
			
			myContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
			myContext->Draw(sprlVCount, 0);

			// Present Backbuffer using Swapchain object
			// Framerate is currently unlocked, we suggest "MSI Afterburner" to track your current FPS and memory usage.
			mySwapChain->Present(0, 0);

			// Free any temp DX handles aquired this frame
			myRenderTargetView->Release();
		}
	}
}