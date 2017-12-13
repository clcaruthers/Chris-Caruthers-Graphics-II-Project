// This file can be used to contain very basic DX11 Loading, Drawing & Clean Up. (Start Here, or Make your own set of classes)
#pragma once
// Include our DX11 middle ware
#include "Gateware Redistributable R4a/Interface/G_Graphics/GDirectX11Surface.h"
// You can clean this up and make it much nicer, otherwise you are stuck in debug 32bit :P
#pragma comment(lib, "Gateware Redistributable R4a/Archive/Win32/Gateware_i386/Debug/GDirectX11Surface.lib")

//OCEAN: loads high poly ocean. OCEAN_LP: loads low poly ocean much faster.
#define OCEAN

// Include DirectX11 for interface access
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <vector>
#include <string>
#include "XTime.h"
#include "DDSTextureLoader.h"
#include "TentacleKnight.h"
#include "Wolf.h"

#include "Trivial_PS.csh"
#include "Trivial_VS.csh"
#include "SkyBox_PS.csh"
#include "Instancing_VS.csh"
#include "Creation_GS.csh"
#include "Geometry_VS.csh"
#include "Waving_VS.csh"
#include "Ocean_PS.csh"
#include "Specular_PS.csh"

using namespace DirectX;
using namespace std;

struct MYVERTEX {
	XMFLOAT4 XYZW;
	XMFLOAT4 UV;
	XMFLOAT4 NORM;
	XMFLOAT4 RGBA;
};

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
	ID3D11InputLayout * InstancingIL;
	ID3D11Buffer * vertBuffer2;
	ID3D11Buffer * lightConstBuff;
	ID3D11Buffer * pLightConBuff;
	ID3D11Buffer * cLightConBuff;
	ID3D11Buffer * cPosConBuff;

	ID3D11Buffer * floorInstanceBuff;

	ID3D11Buffer * sprlVBuff;
	unsigned int sprlVCount;

	ID3D11Buffer * GeoBuff;

	ID3D11Buffer * skyBoxBuff;
	ID3D11Buffer * SBIBuff;
	unsigned int SBVCount;

	/*ID3D11Buffer * TKvertBuffer;
	ID3D11Buffer * TKIndexBuffer;
	unsigned int TKvertCount;*/

	ID3D11Buffer * WolfVertBuffer;
	ID3D11Buffer * WolfIndexBuffer;
	unsigned int WolfVertCount;

	ID3D11Buffer * TieVertBuffer;
	ID3D11Buffer * TieIndexBuffer;
	unsigned int tieVertCount;

	ID3D11Buffer * OceanVertBuffer;
	ID3D11Buffer * OceanIndexBuffer;
	unsigned int oceanVertCount;

	ID3D11Buffer * IslandVertBuffer;
	ID3D11Buffer * IslandIndexBuffer;
	unsigned int IslandVertCount;
	ID3D11Texture2D * IslandTex;
	ID3D11ShaderResourceView * IslandSRV;

	ID3D11Buffer * Palm1VertBuffer;
	ID3D11Buffer * Palm1IndexBuffer;
	unsigned int Palm1VertCount;
	ID3D11Texture2D * Palm1Tex;
	ID3D11ShaderResourceView * Palm1SRV;

	ID3D11Buffer * Palm2VertBuffer;
	ID3D11Buffer * Palm2IndexBuffer;
	unsigned int Palm2VertCount;
	ID3D11Texture2D * Palm2Tex;
	ID3D11ShaderResourceView * Palm2SRV;

	ID3D11Buffer * CFVertBuffer;
	ID3D11Buffer * CFIndexBuffer;
	unsigned int CFVertCount;
	ID3D11Texture2D * CFTex;
	ID3D11ShaderResourceView * CFSRV;

	ID3D11Buffer * FishVertBuffer;
	ID3D11Buffer * FishIndexBuffer;
	unsigned int FishVertCount;
	ID3D11Texture2D * FishTex;
	ID3D11ShaderResourceView * FishSRV;

	ID3D11Buffer * KinglerVertBuffer;
	ID3D11Buffer * KinglerIndexBuffer;
	unsigned int KinglerVertCount;
	ID3D11Texture2D * KinglerTex;
	ID3D11ShaderResourceView * KinglerSRV;

	ID3D11Buffer * OmastarVertBuffer;
	ID3D11Buffer * OmastarIndexBuffer;
	unsigned int OmastarVertCount;
	ID3D11Texture2D * OmastarTex;
	ID3D11ShaderResourceView * OmastarSRV;

	ID3D11Buffer * ExeggVertBuffer;
	ID3D11Buffer * ExeggIndexBuffer;
	unsigned int ExeggVertCount;
	ID3D11Texture2D * ExeggTex;
	ID3D11ShaderResourceView * ExeggSRV;

	ID3D11Buffer * SeadraVertBuffer;
	ID3D11Buffer * SeadraIndexBuffer;
	unsigned int SeadraVertCount;
	ID3D11Texture2D * SeadraTex;
	ID3D11ShaderResourceView * SeadraSRV;

	ID3D11Buffer * FernVertBuffer;
	ID3D11Buffer * FernIndexBuffer;
	unsigned int FernVertCount;
	ID3D11Texture2D * FernTex;
	ID3D11ShaderResourceView * FernSRV;

	ID3D11Buffer * EaselVertBuffer;
	ID3D11Buffer * EaselIndexBuffer;
	unsigned int EaselVertCount;
	ID3D11Texture2D * EaselTex;
	ID3D11ShaderResourceView * EaselSRV;

	ID3D11Buffer * PaintVertBuffer;
	unsigned int PaintVertCount;

	ID3D11VertexShader * vShader;
	ID3D11PixelShader * pShader;
	ID3D11PixelShader * SBpSHader;
	ID3D11VertexShader * IvShader;
	ID3D11GeometryShader * gShader;
	ID3D11VertexShader * GEOvShader;
	ID3D11VertexShader * WAVEvSHader;
	ID3D11PixelShader * OceanpShader;
	ID3D11PixelShader * SpecularpShader;

	ID3D11Buffer * WVOffsetConBuff;

	XTime timeObject;

	XMFLOAT4X4 VIEWMATRIX;
	XMFLOAT4X4 PROJECTIONMATRIX;

	XMFLOAT4X4 pVIEWMATRIX;
	XMFLOAT4X4 pPROJECTIONMATRIX;
	
	XMFLOAT4X4 WORLDMATRIX;
	/*XMFLOAT4X4 TKWORLD;*/
	XMFLOAT4X4 WOLFWORLD;
	XMFLOAT4X4 SPRLWORLD;
	XMFLOAT4X4 SBWORLD;
	XMFLOAT4X4 GEOWORLD;
	XMFLOAT4X4 TIEWORLD;
	XMFLOAT4X4 PLANEWORLD;
	XMFLOAT4X4 ISLANDWORLD;
	XMFLOAT4X4 PALM1WORLD;
	XMFLOAT4X4 PALM2WORLD;
	XMFLOAT4X4 CFWORLD;
	XMFLOAT4X4 FISHWORLD;
	XMFLOAT4X4 FERNWORLD;
	XMFLOAT4X4 PAINTTREEWORLD;
	XMFLOAT4X4 PAINTSKYWORLD;
	XMFLOAT4X4 PAINTWORLD;
	XMFLOAT4X4 EASELWORLD;
	XMFLOAT4X4 KINGLERWORLD;
	XMFLOAT4X4 OMASTARWORLD;
	XMFLOAT4X4 SEADRAWORLD;
	XMFLOAT4X4 EXEGGWORLD;

	float dx = 0;
	float dy = 0;
	float xShift = 0;
	float yShift = 0;
	float zShift = 0;

	float FOV = 65;
	float aspectRatio;

	float zFar = 150.0f;
	float zNear = 0.1f;

	ID3D11Texture2D * floorTex;
	ID3D11ShaderResourceView * floorSRV;
	ID3D11SamplerState * texSampler;

	ID3D11Texture2D * skyBoxTex;
	ID3D11ShaderResourceView * skyBoxSRV;

	//ID3D11Texture2D * TKTex;
	//ID3D11ShaderResourceView * TKSRV;
	
	ID3D11Texture2D * wolfTex;
	ID3D11ShaderResourceView * wolfSRV;

	ID3D11Texture2D * TIETex;
	ID3D11ShaderResourceView * TIESRV;

	ID3D11Texture2D * OceanTex;
	ID3D11ShaderResourceView * OceanSRV;

	ID3D11RasterizerState * BFCController;
	ID3D11BlendState * blendstate;

	ID3D11Texture2D * paintRTVTex;
	ID3D11ShaderResourceView * paintTex;
	ID3D11RenderTargetView * paintRTV;
	ID3D11DepthStencilView * paintDSV;
	ID3D11Texture2D * dsvTex;

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

	struct CONELIGHT_TO_PSHADER {
		XMFLOAT4 lightPos;
		XMFLOAT4 color;
		XMFLOAT4 coneDir;
		float coneRatio;
		XMFLOAT3 pad;
	};

	struct SEND_TO_VRAM {
		XMFLOAT4X4 worldMat;
		XMFLOAT4X4 viewMat;
		XMFLOAT4X4 projMat;
	};

	struct WAVEINFO {
		XMFLOAT4 offst;
	};

	struct CAMPOS {
		XMFLOAT4 pos;
		XMFLOAT4 spec;
	};

	WAVEINFO waveOffset;

	SEND_TO_VRAM toShader;
	DIRLIGHT_TO_PSHADER dirLight;
	POINTLIGHT_TO_PSHADER pLight;
	CONELIGHT_TO_PSHADER cLight;
	CAMPOS cPos;
	bool PLGrow = true;
	bool PLMv = true;
	bool DLMv = true;
	bool CLMv = true;
	bool CLRt = true;
	bool pkmn = false;

public:

	void LoadModel(ID3D11Buffer ** VertBuffer, ID3D11Buffer ** IndexBuffer, unsigned int * vCount, char * filepath);

	// Init
	LetsDrawSomeStuff(GW::SYSTEM::GWindow* attatchPoint);
	// Shutdown
	~LetsDrawSomeStuff();
	// Draw
	void Render();
};

void LetsDrawSomeStuff::LoadModel(ID3D11Buffer ** VertBuffer, ID3D11Buffer ** IndexBuffer, unsigned int * vCount, char * filepath) {
	fstream file;
	file.open(filepath, ios_base::in);
	vector<XMFLOAT4> positions;
	vector<XMFLOAT4> normals;
	vector<XMFLOAT4> UVs;
	vector<unsigned int> posIndecies;
	vector<unsigned int> normIndecies;
	vector<unsigned int> UVIndecies;
	int verts = 0;

	while (!file.eof()) {
		string in;
		file >> in;

		if (in == "v") {
			float x;
			float y;
			float z;
			file >> x;
			file >> y;
			file >> z;

			XMFLOAT4 hold = { x, y, z, 1 };
			positions.push_back(hold);
		}
		else if (in == "vt") {
			float u;
			float v;
			file >> u;
			file >> v;

			v = 1 - v;

			XMFLOAT4 uv = { u, v, 0, 0 };
			UVs.push_back(uv);
		}
		else if (in == "vn") {
			float xn;
			float yn;
			float zn;
			file >> xn;
			file >> yn;
			file >> zn;

			XMFLOAT4 norm = { xn, yn, zn, 0 };
			normals.push_back(norm);
		}
		else if (in == "f") {
			unsigned int p;
			unsigned int t;
			unsigned int n;
			string buf;

			getline(file, buf, '/');
			p = atoi(buf.c_str()) - 1;
			posIndecies.push_back(p);
			getline(file, buf, '/');
			t = atoi(buf.c_str()) - 1;
			UVIndecies.push_back(t);
			getline(file, buf, ' ');
			n = atoi(buf.c_str()) - 1;
			normIndecies.push_back(n);
			verts++;

			getline(file, buf, '/');
			p = atoi(buf.c_str()) - 1;
			posIndecies.push_back(p);
			getline(file, buf, '/');
			t = atoi(buf.c_str()) - 1;
			UVIndecies.push_back(t);
			getline(file, buf, ' ');
			n = atoi(buf.c_str()) - 1;
			normIndecies.push_back(n);
			verts++;

			getline(file, buf, '/');
			p = atoi(buf.c_str()) - 1;
			posIndecies.push_back(p);
			getline(file, buf, '/');
			t = atoi(buf.c_str()) - 1;
			UVIndecies.push_back(t);
			getline(file, buf);
			n = atoi(buf.c_str()) - 1;
			normIndecies.push_back(n);
			verts++;
		}
	}

	MYVERTEX * Vs = new MYVERTEX[verts];
	unsigned int * indecies = new unsigned int[verts];

	for (int i = 0; i < verts; i++) {
		indecies[i] = i;
		Vs[i].XYZW = positions[posIndecies[i]];
		Vs[i].UV = UVs[UVIndecies[i]];
		Vs[i].NORM = normals[normIndecies[i]];
		Vs[i].RGBA = { 1, 1, 1, 1 };
	}

	D3D11_BUFFER_DESC bDesc;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.CPUAccessFlags = NULL;
	bDesc.ByteWidth = sizeof(MYVERTEX) * verts;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = Vs;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	myDevice->CreateBuffer(&bDesc, &initData, VertBuffer);

	D3D11_BUFFER_DESC TKIDesc;
	TKIDesc.Usage = D3D11_USAGE_IMMUTABLE;
	TKIDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	TKIDesc.CPUAccessFlags = NULL;
	TKIDesc.ByteWidth = sizeof(unsigned int) * verts;
	TKIDesc.MiscFlags = 0;
	TKIDesc.StructureByteStride = 0;

	initData.pSysMem = indecies;

	myDevice->CreateBuffer(&TKIDesc, &initData, IndexBuffer);

	*vCount = verts;

	delete[] Vs;
	delete[] indecies;
}

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

			DXGI_SWAP_CHAIN_DESC a;
			mySwapChain->GetDesc(&a);

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

			SBVCount = 8;
			MYVERTEX SkyBoxVerts[8];
			unsigned int SBICount = 36;
			unsigned int SBIndicies[36];

#pragma region

			SkyBoxVerts[0].XYZW.x = -1;
			SkyBoxVerts[0].XYZW.y = 1;
			SkyBoxVerts[0].XYZW.z = 1;
			SkyBoxVerts[0].XYZW.w = 1;
			SkyBoxVerts[0].NORM.x = 0;
			SkyBoxVerts[0].NORM.y = 0;
			SkyBoxVerts[0].NORM.z = 0;
			SkyBoxVerts[0].NORM.w = 0;
			SkyBoxVerts[0].RGBA = SkyBoxVerts[0].NORM;
			SkyBoxVerts[0].UV = SkyBoxVerts[0].NORM;

			SkyBoxVerts[1].XYZW.x = 1;
			SkyBoxVerts[1].XYZW.y = 1;
			SkyBoxVerts[1].XYZW.z = 1;
			SkyBoxVerts[1].XYZW.w = 1;
			SkyBoxVerts[1].NORM.x = 0;
			SkyBoxVerts[1].NORM.y = 0;
			SkyBoxVerts[1].NORM.z = 0;
			SkyBoxVerts[1].NORM.w = 0;
			SkyBoxVerts[1].RGBA = SkyBoxVerts[0].NORM;
			SkyBoxVerts[1].UV = SkyBoxVerts[0].NORM;

			SkyBoxVerts[2].XYZW.x = 1;
			SkyBoxVerts[2].XYZW.y = 1;
			SkyBoxVerts[2].XYZW.z = -1;
			SkyBoxVerts[2].XYZW.w = 1;
			SkyBoxVerts[2].NORM.x = 0;
			SkyBoxVerts[2].NORM.y = 0;
			SkyBoxVerts[2].NORM.z = 0;
			SkyBoxVerts[2].NORM.w = 0;
			SkyBoxVerts[2].RGBA = SkyBoxVerts[0].NORM;
			SkyBoxVerts[2].UV = SkyBoxVerts[0].NORM;

			SkyBoxVerts[3].XYZW.x = -1;
			SkyBoxVerts[3].XYZW.y = 1;
			SkyBoxVerts[3].XYZW.z = -1;
			SkyBoxVerts[3].XYZW.w = 1;
			SkyBoxVerts[3].NORM.x = 0;
			SkyBoxVerts[3].NORM.y = 0;
			SkyBoxVerts[3].NORM.z = 0;
			SkyBoxVerts[3].NORM.w = 0;
			SkyBoxVerts[3].RGBA = SkyBoxVerts[0].NORM;
			SkyBoxVerts[3].UV = SkyBoxVerts[0].NORM;

			SkyBoxVerts[4].XYZW.x = -1;
			SkyBoxVerts[4].XYZW.y = -1;
			SkyBoxVerts[4].XYZW.z = 1;
			SkyBoxVerts[4].XYZW.w = 1;
			SkyBoxVerts[4].NORM.x = 0;
			SkyBoxVerts[4].NORM.y = 0;
			SkyBoxVerts[4].NORM.z = 0;
			SkyBoxVerts[4].NORM.w = 0;
			SkyBoxVerts[4].RGBA = SkyBoxVerts[0].NORM;
			SkyBoxVerts[4].UV = SkyBoxVerts[0].NORM;

			SkyBoxVerts[5].XYZW.x = 1;
			SkyBoxVerts[5].XYZW.y = -1;
			SkyBoxVerts[5].XYZW.z = 1;
			SkyBoxVerts[5].XYZW.w = 1;
			SkyBoxVerts[5].NORM.x = 0;
			SkyBoxVerts[5].NORM.y = 0;
			SkyBoxVerts[5].NORM.z = 0;
			SkyBoxVerts[5].NORM.w = 0;
			SkyBoxVerts[5].RGBA = SkyBoxVerts[0].NORM;
			SkyBoxVerts[5].UV = SkyBoxVerts[0].NORM;

			SkyBoxVerts[6].XYZW.x = 1;
			SkyBoxVerts[6].XYZW.y = -1;
			SkyBoxVerts[6].XYZW.z = -1;
			SkyBoxVerts[6].XYZW.w = 1;
			SkyBoxVerts[6].NORM.x = 0;
			SkyBoxVerts[6].NORM.y = 0;
			SkyBoxVerts[6].NORM.z = 0;
			SkyBoxVerts[6].NORM.w = 0;
			SkyBoxVerts[6].RGBA = SkyBoxVerts[0].NORM;
			SkyBoxVerts[6].UV = SkyBoxVerts[0].NORM;

			SkyBoxVerts[7].XYZW.x = -1;
			SkyBoxVerts[7].XYZW.y = -1;
			SkyBoxVerts[7].XYZW.z = -1;
			SkyBoxVerts[7].XYZW.w = 1;
			SkyBoxVerts[7].NORM.x = 0;
			SkyBoxVerts[7].NORM.y = 0;
			SkyBoxVerts[7].NORM.z = 0;
			SkyBoxVerts[7].NORM.w = 0;
			SkyBoxVerts[7].RGBA = SkyBoxVerts[0].NORM;
			SkyBoxVerts[7].UV = SkyBoxVerts[0].NORM;

			SBIndicies[0] = 0;
			SBIndicies[1] = 1;
			SBIndicies[2] = 4;

			SBIndicies[3] = 1;
			SBIndicies[4] = 5;
			SBIndicies[5] = 4;
			
			SBIndicies[6] = 1;
			SBIndicies[7] = 2;
			SBIndicies[8] = 5;
			
			SBIndicies[9] = 2;
			SBIndicies[10] = 6;
			SBIndicies[11] = 5;
			
			SBIndicies[12] = 2;
			SBIndicies[13] = 3;
			SBIndicies[14] = 6;

			SBIndicies[15] = 3;
			SBIndicies[16] = 7;
			SBIndicies[17] = 6;

			SBIndicies[18] = 3;
			SBIndicies[19] = 0;
			SBIndicies[20] = 4;

			SBIndicies[21] = 3;
			SBIndicies[22] = 4;
			SBIndicies[23] = 7;

			SBIndicies[24] = 1;
			SBIndicies[25] = 0;
			SBIndicies[26] = 2;

			SBIndicies[27] = 0;
			SBIndicies[28] = 3;
			SBIndicies[29] = 2;

			SBIndicies[30] = 4;
			SBIndicies[31] = 5;
			SBIndicies[32] = 6;

			SBIndicies[33] = 4;
			SBIndicies[34] = 6;
			SBIndicies[35] = 7;

#pragma endregion

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

			PaintVertCount = 6;
			MYVERTEX pVerts[6];
#pragma region

			pVerts[0].XYZW = { -0.5f, 1, 0, 1 };
			pVerts[0].NORM = { 0, 0, 1, 0 };
			pVerts[0].UV = { 0, 0, 0, 0 };
			pVerts[0].RGBA = { 1, 1, 1, 1 };

			pVerts[1].XYZW = { 0.5f, 1, 0, 1 };
			pVerts[1].NORM = { 0, 0, 1, 0 };
			pVerts[1].UV = { 1, 0, 0, 0 };
			pVerts[1].RGBA = { 1, 1, 1, 1 };

			pVerts[2].XYZW = { -0.5f, -1, 0, 1 };
			pVerts[2].NORM = { 0, 0, 1, 0 };
			pVerts[2].UV = { 0, 1, 0, 0 };
			pVerts[2].RGBA = { 1, 1, 1, 1 };

			pVerts[3].XYZW = { 0.5f, 1, 0, 1 };
			pVerts[3].NORM = { 0, 0, 1, 0 };
			pVerts[3].UV = { 1, 0, 0, 0 };
			pVerts[3].RGBA = { 1, 1, 1, 1 };

			pVerts[4].XYZW = { 0.5f, -1, 0, 1 };
			pVerts[4].NORM = { 0, 0, 1, 0 };
			pVerts[4].UV = { 1, 1, 0, 0 };
			pVerts[4].RGBA = { 1, 1, 1, 1 };

			pVerts[5].XYZW = { -0.5f, -1, 0, 1 };
			pVerts[5].NORM = { 0, 0, 1, 0 };
			pVerts[5].UV = { 0, 1, 0, 0 };
			pVerts[5].RGBA = { 1, 1, 1, 1 };

#pragma endregion

			MYVERTEX geometry[1];
			
			geometry[0].XYZW.x = 0;
			geometry[0].XYZW.y = 0;
			geometry[0].XYZW.z = 0;
			geometry[0].XYZW.w = 1;

			geometry[0].UV.x = 0;
			geometry[0].UV.y = 0;
			geometry[0].UV.z = 0;
			geometry[0].UV.w = 0;

			geometry[0].NORM.x = 0;
			geometry[0].NORM.y = 0;
			geometry[0].NORM.z = -1;
			geometry[0].NORM.w = 0;

			geometry[0].RGBA.x = 1;
			geometry[0].RGBA.y = 0;
			geometry[0].RGBA.z = 1;
			geometry[0].RGBA.w = 0;

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
			CreateDDSTextureFromFile(myDevice, L"IslandCube.dds", (ID3D11Resource**)&skyBoxTex, &skyBoxSRV);
			CreateDDSTextureFromFile(myDevice, L"TIE_Interceptor.dds", (ID3D11Resource**)&TIETex, &TIESRV);
			CreateDDSTextureFromFile(myDevice, L"ocean.dds", (ID3D11Resource**)&OceanTex, &OceanSRV);
			CreateDDSTextureFromFile(myDevice, L"field.dds", (ID3D11Resource**)&IslandTex, &IslandSRV);
			CreateDDSTextureFromFile(myDevice, L"palmtree.dds", (ID3D11Resource**)&Palm1Tex, &Palm1SRV);
			CreateDDSTextureFromFile(myDevice, L"fire.dds", (ID3D11Resource**)&CFTex, &CFSRV);
			CreateDDSTextureFromFile(myDevice, L"Rudd_Fish_diff.dds", (ID3D11Resource**)&FishTex, &FishSRV);
			CreateDDSTextureFromFile(myDevice, L"fern_diff.dds", (ID3D11Resource**)&FernTex, &FernSRV);
			CreateDDSTextureFromFile(myDevice, L"tropical_plant_diff.dds", (ID3D11Resource**)&Palm2Tex, &Palm2SRV);
			CreateDDSTextureFromFile(myDevice, L"wood.dds", (ID3D11Resource**)&EaselTex, &EaselSRV);
			CreateDDSTextureFromFile(myDevice, L"Kingler.dds", (ID3D11Resource**)&KinglerTex, &KinglerSRV);
			CreateDDSTextureFromFile(myDevice, L"omastar.dds", (ID3D11Resource**)&OmastarTex, &OmastarSRV);
			CreateDDSTextureFromFile(myDevice, L"seadra.dds", (ID3D11Resource**)&SeadraTex, &SeadraSRV);
			CreateDDSTextureFromFile(myDevice, L"exeggutor.dds", (ID3D11Resource**)&ExeggTex, &ExeggSRV);

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

			bDesc.ByteWidth = sizeof(MYVERTEX);
			D3D11_SUBRESOURCE_DATA geoInit;
			geoInit.pSysMem = &geometry;
			geoInit.SysMemPitch = 0;
			geoInit.SysMemSlicePitch = 0;

			myDevice->CreateBuffer(&bDesc, &geoInit, &GeoBuff);

			//spiral
			bDesc.ByteWidth = sizeof(MYVERTEX) * sprlVCount;
			D3D11_SUBRESOURCE_DATA sprlInit;
			sprlInit.pSysMem = spiralVerts;
			sprlInit.SysMemPitch = 0;
			sprlInit.SysMemSlicePitch = 0;

			myDevice->CreateBuffer(&bDesc, &sprlInit, &sprlVBuff);

			XMFLOAT4 instances[25];

#pragma region
			instances[0].x = -2;
			instances[0].y = 0;
			instances[0].z = 2;
			instances[0].w = 0;

			instances[1].x = 0;
			instances[1].y = 0;
			instances[1].z = 2;
			instances[1].w = 0;

			instances[2].x = 2;
			instances[2].y = 0;
			instances[2].z = 2;
			instances[2].w = 0;

			instances[3].x = -2;
			instances[3].y = 0;
			instances[3].z = 0;
			instances[3].w = 0;

			instances[4].x = 2;
			instances[4].y = 0;
			instances[4].z = 0;
			instances[4].w = 0;

			instances[5].x = -2;
			instances[5].y = 0;
			instances[5].z = -2;
			instances[5].w = 0;

			instances[6].x = 0;
			instances[6].y = 0;
			instances[6].z = -2;
			instances[6].w = 0;

			instances[7].x = 2;
			instances[7].y = 0;
			instances[7].z = -2;
			instances[7].w = 0;

			instances[8].x = -4;
			instances[8].y = 0;
			instances[8].z = 4;
			instances[8].w = 0;

			instances[9].x = -2;
			instances[9].y = 0;
			instances[9].z = 4;
			instances[9].w = 0;

			instances[10].x = 0;
			instances[10].y = 0;
			instances[10].z = 4;
			instances[10].w = 0;

			instances[11].x = 2;
			instances[11].y = 0;
			instances[11].z = 4;
			instances[11].w = 0;

			instances[12].x = 4;
			instances[12].y = 0;
			instances[12].z = 4;
			instances[12].w = 0;

			instances[13].x = -4;
			instances[13].y = 0;
			instances[13].z = 2;
			instances[13].w = 0;

			instances[14].x = 4;
			instances[14].y = 0;
			instances[14].z = 2;
			instances[14].w = 0;

			instances[15].x = -4;
			instances[15].y = 0;
			instances[15].z = 0;
			instances[15].w = 0;

			instances[16].x = 4;
			instances[16].y = 0;
			instances[16].z = 0;
			instances[16].w = 0;

			instances[17].x = -4;
			instances[17].y = 0;
			instances[17].z = -2;
			instances[17].w = 0;

			instances[18].x = 4;
			instances[18].y = 0;
			instances[18].z = -2;
			instances[18].w = 0;

			instances[19].x = -4;
			instances[19].y = 0;
			instances[19].z = -4;
			instances[19].w = 0;

			instances[20].x = -2;
			instances[20].y = 0;
			instances[20].z = -4;
			instances[20].w = 0;

			instances[21].x = 0;
			instances[21].y = 0;
			instances[21].z = -4;
			instances[21].w = 0;

			instances[22].x = 2;
			instances[22].y = 0;
			instances[22].z = -4;
			instances[22].w = 0;

			instances[23].x = 4;
			instances[23].y = 0;
			instances[23].z = -4;
			instances[23].w = 0;

			instances[24].x = 0;
			instances[24].y = 0;
			instances[24].z = 0;
			instances[24].w = 0;

#pragma endregion

			bDesc.ByteWidth = sizeof(XMFLOAT4) * 25;
			D3D11_SUBRESOURCE_DATA instncInit;
			instncInit.pSysMem = &instances[0];
			instncInit.SysMemPitch = 0;
			instncInit.SysMemSlicePitch = 0;

			myDevice->CreateBuffer(&bDesc, &instncInit, &floorInstanceBuff);
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

			bDesc.ByteWidth = sizeof(MYVERTEX) * SBVCount;
			wolfInit.pSysMem = &SkyBoxVerts;
			myDevice->CreateBuffer(&bDesc, &wolfInit, &skyBoxBuff);

			TKIDesc.ByteWidth = sizeof(unsigned int) * SBICount;
			wolfInit.pSysMem = &SBIndicies;
			myDevice->CreateBuffer(&TKIDesc, &wolfInit, &SBIBuff);

			bDesc.ByteWidth = sizeof(MYVERTEX) * PaintVertCount;
			wolfInit.pSysMem = &pVerts;
			myDevice->CreateBuffer(&bDesc, &wolfInit, &PaintVertBuffer);
			

			// TODO: PART 2 STEP 7
			myDevice->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &vShader);
			myDevice->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pShader);
			myDevice->CreatePixelShader(SkyBox_PS, sizeof(SkyBox_PS), NULL, &SBpSHader);
			myDevice->CreateVertexShader(Instancing_VS, sizeof(Instancing_VS), NULL, &IvShader);
			myDevice->CreateGeometryShader(Creation_GS, sizeof(Creation_GS), NULL, &gShader);
			myDevice->CreateVertexShader(Geometry_VS, sizeof(Geometry_VS), NULL, &GEOvShader);
			myDevice->CreateVertexShader(Waving_VS, sizeof(Waving_VS), NULL, &WAVEvSHader);
			myDevice->CreatePixelShader(Ocean_PS, sizeof(Ocean_PS), NULL, &OceanpShader);
			myDevice->CreatePixelShader(Specular_PS, sizeof(Specular_PS), NULL, &SpecularpShader);
			// TODO: PART 2 STEP 8a

			D3D11_INPUT_ELEMENT_DESC IED[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
				D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT , 0, 16,
				D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT , 0, 32,
				D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48,
				D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,
				D3D11_INPUT_PER_INSTANCE_DATA, 1}
			};

			// TODO: PART 2 STEP 8b
			myDevice->CreateInputLayout(IED, 4, Trivial_VS, sizeof(Trivial_VS), &IL);
			myDevice->CreateInputLayout(IED, 5, Instancing_VS, sizeof(Instancing_VS), &InstancingIL);

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

			bDesc2.ByteWidth = sizeof(CONELIGHT_TO_PSHADER);

			myDevice->CreateBuffer(&bDesc2, NULL, &cLightConBuff);

			bDesc2.ByteWidth = sizeof(CAMPOS);

			myDevice->CreateBuffer(&bDesc2, NULL, &cPosConBuff);

			bDesc2.ByteWidth = sizeof(waveOffset);

			myDevice->CreateBuffer(&bDesc2, NULL, &WVOffsetConBuff);

			LoadModel(&TieVertBuffer, &TieIndexBuffer, &tieVertCount, "TIE.obj");
			LoadModel(&IslandVertBuffer, &IslandIndexBuffer, &IslandVertCount, "island.obj");
			LoadModel(&Palm1VertBuffer, &Palm1IndexBuffer, &Palm1VertCount, "palm_tree_lowpoly.obj");
			LoadModel(&CFVertBuffer, &CFIndexBuffer, &CFVertCount, "campfire.obj");
			LoadModel(&FishVertBuffer, &FishIndexBuffer, &FishVertCount, "Rudd_Fish.obj");
			LoadModel(&FernVertBuffer, &FernIndexBuffer, &FernVertCount, "fern.obj");
			LoadModel(&Palm2VertBuffer, &Palm2IndexBuffer, &Palm2VertCount, "tropical_plant.obj");
			LoadModel(&EaselVertBuffer, &EaselIndexBuffer, &EaselVertCount, "easel.obj");
			LoadModel(&KinglerVertBuffer, &KinglerIndexBuffer, &KinglerVertCount, "Kingler.obj");
			LoadModel(&OmastarVertBuffer, &OmastarIndexBuffer, &OmastarVertCount, "omastar.obj");
			LoadModel(&SeadraVertBuffer, &SeadraIndexBuffer, &SeadraVertCount, "seadra.obj");
			LoadModel(&ExeggVertBuffer, &ExeggIndexBuffer, &ExeggVertCount, "exeggutor.obj");

#ifdef OCEAN_LP
			LoadModel(&OceanVertBuffer, &OceanIndexBuffer, &oceanVertCount, "Ocean_lowpoly.obj");
#endif

#ifdef OCEAN
			LoadModel(&OceanVertBuffer, &OceanIndexBuffer, &oceanVertCount, "Ocean.obj");
#endif

			//WORLD MATRICES CREATION
			XMStoreFloat4x4(&WORLDMATRIX, XMMatrixTranslation(0, -1, 0));
			/*XMStoreFloat4x4(&TKWORLD, XMMatrixTranslation(0, 0.5f, 0) * XMMatrixScaling(0.5f, 0.5f, 0.5f));*/
			XMStoreFloat4x4(&WOLFWORLD, XMMatrixTranslation(0.5f, 0, 0));
			XMStoreFloat4x4(&SPRLWORLD, XMMatrixTranslation(0.45f, 0.68f, 0.8f));
			XMStoreFloat4x4(&GEOWORLD, XMMatrixTranslation(-3, 0, 3));
			XMStoreFloat4x4(&TIEWORLD, XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixRotationY(XMConvertToRadians(120)) *  XMMatrixTranslation(-1, 8, -65));
			XMStoreFloat4x4(&PLANEWORLD, XMMatrixScaling(5, 5, 5));
			XMStoreFloat4x4(&ISLANDWORLD, XMMatrixScaling(1.45f, 1.45f, 1.45f) * XMMatrixTranslation(0, -0.8f, 0));
			XMStoreFloat4x4(&PALM1WORLD, XMMatrixScaling(0.5f ,0.5f, 0.5f) * XMMatrixTranslation(-0.5f, 0.5f, 0.5f));
			XMStoreFloat4x4(&CFWORLD, XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixTranslation(1, 1.3f, 0));
			XMStoreFloat4x4(&FISHWORLD, XMMatrixScaling(0.6f, 0.6f, 0.6f) * XMMatrixTranslation(6, -0.5f, -6));
			XMStoreFloat4x4(&FERNWORLD, XMMatrixTranslation(-0.7f, 1, 1.2f));
			XMStoreFloat4x4(&PALM2WORLD, XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(1.5f, 1, -2));
			XMStoreFloat4x4(&PAINTTREEWORLD, XMMatrixScaling(0.7f, 1.6f, 0.7f) * XMMatrixTranslation(-7, -6, 18));
			XMStoreFloat4x4(&EASELWORLD, XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(20)) * XMMatrixTranslation(1.2f, 1, 3.6f));
			XMStoreFloat4x4(&PAINTWORLD, XMMatrixScaling(14, 14, 14) * XMMatrixRotationX(XMConvertToRadians(-20)) * XMMatrixTranslation(0, 47, 4.5f) * XMLoadFloat4x4(&EASELWORLD));
			XMStoreFloat4x4(&PAINTSKYWORLD, XMMatrixScaling(30, 30, 30));
			XMStoreFloat4x4(&KINGLERWORLD, XMMatrixRotationX(XMConvertToRadians(-15)) * XMMatrixScaling(0.3f, 0.3f, 0.3f) * XMMatrixTranslation(0, 1, -2));
			XMStoreFloat4x4(&OMASTARWORLD, XMMatrixRotationX(XMConvertToRadians(-18)) * XMMatrixRotationY(XMConvertToRadians(180)) * XMMatrixScaling(1.2f, 1.2f, 1.2f) * XMMatrixTranslation(-1, 0.15f, 6.3f));
			XMStoreFloat4x4(&SEADRAWORLD, XMMatrixTranslation(-7, -0.2f, 2));
			XMStoreFloat4x4(&EXEGGWORLD, XMMatrixRotationY(XMConvertToRadians(75)) * XMMatrixScaling(0.4f, 0.4f, 0.4f) * XMMatrixTranslation(2.5f, 1.25f, 1.5f));

			XMStoreFloat4x4(&VIEWMATRIX, XMMatrixTranslation(0, -2, -1.0f));

			XMStoreFloat4x4(&pVIEWMATRIX, XMMatrixIdentity());
			float paintAR = 250.0f / 500.0f;
			XMStoreFloat4x4(&pPROJECTIONMATRIX, XMMatrixPerspectiveFovLH(XMConvertToRadians(90), paintAR, zNear, zFar));

			pLight.color = { 1, 0.3f, 0, 1 };
			pLight.lightPos = { 1, 1.7f, 0, 1 };
			pLight.radius = 20;

			dirLight.light = { -0.43f, -0.3f, -1, 0 };
			dirLight.color = { 1.0f, 0.4f, 0.2f, 1.0f };

			cLight.color = { 1, 1, 0, 1 };
			cLight.lightPos = { 0, 3, 0, 1 };
			cLight.coneDir = { 0, 1, -0.7f, 0 };
			cLight.coneRatio = 0.93f;

			cPos.spec.x = 128;

			waveOffset.offst.x = 6.28f;
			waveOffset.offst.y = 0;
			waveOffset.offst.z = 0;
			waveOffset.offst.w = 0;


			D3D11_RASTERIZER_DESC rDesc;
			rDesc.CullMode = D3D11_CULL_NONE;
			rDesc.DepthBias = 0;
			rDesc.FillMode = D3D11_FILL_SOLID;
			rDesc.FrontCounterClockwise = false;
			rDesc.SlopeScaledDepthBias = 0.0f;
			rDesc.DepthBiasClamp = 0.0f;
			rDesc.DepthClipEnable = true;
			rDesc.ScissorEnable = false;
			rDesc.MultisampleEnable = true;
			rDesc.AntialiasedLineEnable = true;

			myDevice->CreateRasterizerState(&rDesc, &BFCController);

			D3D11_BLEND_DESC blDesc;
			blDesc.AlphaToCoverageEnable = true;
			blDesc.IndependentBlendEnable = false;
			blDesc.RenderTarget[0].BlendEnable = true;
			blDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
			blDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
			blDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			blDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
			blDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
			blDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			blDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			/*blDesc.RenderTarget[1].BlendEnable = true;
			blDesc.RenderTarget[1].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
			blDesc.RenderTarget[1].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
			blDesc.RenderTarget[1].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			blDesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
			blDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
			blDesc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
			blDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;*/

			myDevice->CreateBlendState(&blDesc, &blendstate);

			float h[] = { 1, 1, 1, 1 };
			myContext->OMSetBlendState(blendstate, h, 0xFFFFFFFF);

			//Render to Texture Set-up
			D3D11_TEXTURE2D_DESC t2dDesc;
			ZeroMemory(&t2dDesc, sizeof(t2dDesc));
			t2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			t2dDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			t2dDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			t2dDesc.ArraySize = 1;
			t2dDesc.CPUAccessFlags = 0;
			t2dDesc.Height = 500;
			t2dDesc.Width = 250;
			t2dDesc.MipLevels = 1;
			t2dDesc.SampleDesc.Count = 1;
			t2dDesc.Usage = D3D11_USAGE_DEFAULT;
			myDevice->CreateTexture2D(&t2dDesc, 0, &paintRTVTex);

			t2dDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			t2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			t2dDesc.MiscFlags = 0;
			myDevice->CreateTexture2D(&t2dDesc, 0, &dsvTex);

			t2dDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			ZeroMemory(&rtvDesc, sizeof(rtvDesc));
			rtvDesc.Format = t2dDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			myDevice->CreateRenderTargetView(paintRTVTex, &rtvDesc, &paintRTV);

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = t2dDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture1D.MipLevels = 1;
			myDevice->CreateShaderResourceView(paintRTVTex, &srvDesc, &paintTex);

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			dsvDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			dsvDesc.Flags = 0;
			myDevice->CreateDepthStencilView(dsvTex, &dsvDesc, &paintDSV);

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
	SBpSHader->Release();
	IvShader->Release();
	gShader->Release();
	GEOvShader->Release();
	WAVEvSHader->Release();
	IL->Release();
	InstancingIL->Release();
	OceanpShader->Release();
	SpecularpShader->Release();
	

	floorTex->Release();
	texSampler->Release();
	floorSRV->Release();
	floorInstanceBuff->Release();

	/*TKTex->Release();
	TKSRV->Release();*/

	lightConstBuff->Release();
	pLightConBuff->Release();
	cLightConBuff->Release();
	cPosConBuff->Release();

	wolfTex->Release();
	wolfSRV->Release();

	skyBoxBuff->Release();
	skyBoxSRV->Release();
	skyBoxTex->Release();
	SBIBuff->Release();

	TieVertBuffer->Release();
	TieIndexBuffer->Release();
	TIETex->Release();
	TIESRV->Release();

	OceanVertBuffer->Release();
	OceanIndexBuffer->Release();
	OceanTex->Release();
	OceanSRV->Release();

	IslandVertBuffer->Release();
	IslandIndexBuffer->Release();
	IslandTex->Release();
	IslandSRV->Release();

	Palm1VertBuffer->Release();
	Palm1IndexBuffer->Release();
	Palm1Tex->Release();
	Palm1SRV->Release();

	Palm2VertBuffer->Release();
	Palm2IndexBuffer->Release();
	Palm2Tex->Release();
	Palm2SRV->Release();

	CFVertBuffer->Release();
	CFIndexBuffer->Release();
	CFTex->Release();
	CFSRV->Release();

	FishVertBuffer->Release();
	FishIndexBuffer->Release();
	FishTex->Release();
	FishSRV->Release();

	FernVertBuffer->Release();
	FernIndexBuffer->Release();
	FernTex->Release();
	FernSRV->Release();

	KinglerVertBuffer->Release();
	KinglerIndexBuffer->Release();
	KinglerTex->Release();
	KinglerSRV->Release();

	OmastarVertBuffer->Release();
	OmastarIndexBuffer->Release();
	OmastarTex->Release();
	OmastarSRV->Release();

	ExeggVertBuffer->Release();
	ExeggIndexBuffer->Release();
	ExeggTex->Release();
	ExeggSRV->Release();

	SeadraVertBuffer->Release();
	SeadraIndexBuffer->Release();
	SeadraTex->Release();
	SeadraSRV->Release();

	EaselVertBuffer->Release();
	EaselIndexBuffer->Release();
	EaselTex->Release();
	EaselSRV->Release();

	BFCController->Release();
	blendstate->Release();

	GeoBuff->Release();

	WVOffsetConBuff->Release();

	paintRTV->Release();
	paintDSV->Release();
	paintTex->Release();
	paintRTVTex->Release();
	dsvTex->Release();

	PaintVertBuffer->Release();


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

			myContext->RSSetState(BFCController);

			timeObject.Signal();
			double timestep = timeObject.Delta();

			if (GetAsyncKeyState(VK_DOWN)) {
				dx = timestep;
			}
			else if (GetAsyncKeyState(VK_UP)) {
				dx = -timestep;
			}
			else {
				dx = 0;
			}

			if (GetAsyncKeyState(VK_RIGHT)) {
				dy = timestep;
			}
			else if (GetAsyncKeyState(VK_LEFT)) {
				dy = -timestep;
			}
			else {
				dy = 0;
			}

			XMMATRIX temp = XMLoadFloat4x4(&VIEWMATRIX);
			temp = XMMatrixInverse(nullptr, temp);
			XMStoreFloat4x4(&VIEWMATRIX, temp);

			float moveSpd = 5;

			if (GetAsyncKeyState(VK_SPACE)) {
				XMMATRIX translation = XMMatrixTranslation(0.0f, moveSpd * timestep , 0.0f);
				XMMATRIX temp_camera = XMLoadFloat4x4(&VIEWMATRIX);
				XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
				XMStoreFloat4x4(&VIEWMATRIX, result);
			}
			if (GetAsyncKeyState(VK_LCONTROL)) {
				XMMATRIX translation = XMMatrixTranslation(0.0f, -moveSpd * timestep, 0.0f);
				XMMATRIX temp_camera = XMLoadFloat4x4(&VIEWMATRIX);
				XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
				XMStoreFloat4x4(&VIEWMATRIX, result);
			}
			if (GetAsyncKeyState('W')) {
				XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, moveSpd * timestep);
				XMMATRIX temp_camera = XMLoadFloat4x4(&VIEWMATRIX);
				XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
				XMStoreFloat4x4(&VIEWMATRIX, result);
			}
			if (GetAsyncKeyState('S')) {
				XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -moveSpd * timestep);
				XMMATRIX temp_camera = XMLoadFloat4x4(&VIEWMATRIX);
				XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
				XMStoreFloat4x4(&VIEWMATRIX, result);
			}
			if (GetAsyncKeyState('A')) {
				XMMATRIX translation = XMMatrixTranslation(-moveSpd * timestep, 0.0f, 0.0f);
				XMMATRIX temp_camera = XMLoadFloat4x4(&VIEWMATRIX);
				XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
				XMStoreFloat4x4(&VIEWMATRIX, result);
			}
			if (GetAsyncKeyState('D')) {
				XMMATRIX translation = XMMatrixTranslation(moveSpd * timestep, 0.0f, 0.0f);
				XMMATRIX temp_camera = XMLoadFloat4x4(&VIEWMATRIX);
				XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
				XMStoreFloat4x4(&VIEWMATRIX, result);
			}

			if (GetAsyncKeyState('Q'))
				FOV -= 40 * timestep;
			if (GetAsyncKeyState('E'))
				FOV += 40 * timestep;

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

			if (GetAsyncKeyState(VK_NUMPAD7) & 0x1) {
				if (cPos.spec.x < 2048) {
					cPos.spec.x *= 2;
				}
			}
			if (GetAsyncKeyState(VK_NUMPAD4) & 0x1) {
				if (cPos.spec.x > 2) {
					cPos.spec.x /= 2;
				}
			}

			if (GetAsyncKeyState(VK_NUMPAD1) & 0x1) {
				cLight.coneDir.y *= -1;
			}

			if (GetAsyncKeyState('P') & 0x1) {
				pkmn = !pkmn;
			}

			if (zFar < 1)
				zFar = 1;
			if (zNear < 0.1f)
				zNear = 0.1f;
			if (zNear >= zFar)
				zNear = zFar - 0.9f;

			if (FOV < 1)
				FOV = 1;
			else if (FOV > 150)
				FOV = 150;

			waveOffset.offst.x += 2 * timestep;

			if (waveOffset.offst.x >= 12.56f)
				waveOffset.offst.x -= 6.28f;

			if (dx != 0 || dy != 0) {
				XMFLOAT4 pos = XMFLOAT4(VIEWMATRIX._41, VIEWMATRIX._42, VIEWMATRIX._43, VIEWMATRIX._44);

				VIEWMATRIX._41 = 0;
				VIEWMATRIX._42 = 0;
				VIEWMATRIX._43 = 0;

				XMMATRIX rotX = XMMatrixRotationX(dx);
				XMMATRIX rotY = XMMatrixRotationY(dy);

				XMMATRIX temp_camera = XMLoadFloat4x4(&VIEWMATRIX);
				temp_camera = XMMatrixMultiply(rotX, temp_camera);
				temp_camera = XMMatrixMultiply(temp_camera, rotY);

				XMStoreFloat4x4(&VIEWMATRIX, temp_camera);

				VIEWMATRIX._41 = pos.x;
				VIEWMATRIX._42 = pos.y;
				VIEWMATRIX._43 = pos.z;
			}

			//SET SKYBOX WORLD POS
			XMFLOAT4X4 hold;
			XMStoreFloat4x4(&hold, temp);
			XMStoreFloat4x4(&SBWORLD, XMMatrixTranslation(hold._41, hold._42, hold._43));
			cPos.pos = { hold._41, hold._42, hold._43, hold._44 };

			temp = XMLoadFloat4x4(&VIEWMATRIX);
			temp = XMMatrixInverse(nullptr, temp);
			XMStoreFloat4x4(&VIEWMATRIX, temp);

			/*XMMATRIX xr = XMMatrixRotationX(XMConvertToRadians(xRot));
			XMMATRIX yr = XMMatrixRotationY(XMConvertToRadians(yRot));
			XMMATRIX rot = xr * yr;

			XMMATRIX translate = XMMatrixTranslation(xShift, yShift, zShift);

			XMMATRIX tempView = translate * rot;
			XMFLOAT4X4 hold;
			XMStoreFloat4x4(&hold, tempView);
			XMStoreFloat4x4(&SBWORLD, XMMatrixTranslation(hold._41, hold._42, hold._43));
			tempView = XMMatrixInverse(nullptr, tempView);
			XMStoreFloat4x4(&VIEWMATRIX, tempView);*/

			// Set active target for drawing, all array based D3D11 functions should use a syntax similar to below
			
			ID3D11RenderTargetView* const targets[] = { myRenderTargetView, paintRTV };
			myContext->OMSetRenderTargets(1, &targets[0], myDepthStencilView);

			// Clear the screen to dark green
			const float d_green[] = { 0, 0.5f, 0, 1 };
			myContext->ClearRenderTargetView(myRenderTargetView, d_green);
			
			// TODO: Set your shaders, Update & Set your constant buffers, Attatch your vertex & index buffers, Set your InputLayout & Topology & Draw!

			toShader.worldMat = SBWORLD;
			toShader.viewMat = VIEWMATRIX;
			toShader.projMat = PROJECTIONMATRIX;

			/*if (DLMv) {
				dirLight.light.y += 0.8f * timestep;
			}
			else {
				dirLight.light.y -= 0.8f * timestep;
			}

			if (dirLight.light.y >= 1) {
				DLMv = false;
			}
			else if (dirLight.light.y <= -1) {
				DLMv = true;
			}*/


			if (PLGrow) {
				pLight.radius += 1.0f * timestep;
			}
			else {
				pLight.radius -= 1.0f * timestep;
			}

			if (pLight.radius >= 22.0f) {
				PLGrow = false;
			}
			else if (pLight.radius < 18.0f) {
				PLGrow = true;
			}


			/*if (PLMv) {
				pLight.lightPos.z += 2 * timestep;
			}
			else {
				pLight.lightPos.z -= 2 * timestep;
			}

			if (pLight.lightPos.z >= 1) {
				PLMv = false;
			}
			else if (pLight.lightPos.z <= -1) {
				PLMv = true;
			}*/

			/*if (CLMv) {
				cLight.lightPos.x += 2 * timestep;
			}
			else {
				cLight.lightPos.x -= 2 * timestep;
			}*/

			/*if (cLight.lightPos.x >= 5) {
				CLMv = false;
			}
			else if (cLight.lightPos.x <= -5) {
				CLMv = true;
			}*/

			if (CLRt) {
				cLight.coneDir.x += 0.5f * timestep;
			}
			else {
				cLight.coneDir.x -= 0.5f * timestep;
			}

			if (cLight.coneDir.x >= 1) {
				CLRt = false;
			}
			else if (cLight.coneDir.x <= -1) {
				CLRt = true;
			}

			

			//set texture SRVs						//0		//1			//2		//3		//4			//5		//6			//7		//8		//9		//10		//11	//12		//13		//14		//15	//16
			ID3D11ShaderResourceView * SRVs[] = { floorSRV, wolfSRV, skyBoxSRV, TIESRV, OceanSRV, IslandSRV, Palm1SRV, CFSRV, FishSRV, FernSRV, Palm2SRV, paintTex, EaselSRV, KinglerSRV, OmastarSRV, SeadraSRV, ExeggSRV };

			//myContext->RSSetViewports(1, &viewport);
			D3D11_MAPPED_SUBRESOURCE mapResource;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->VSSetConstantBuffers(0, 1, &vertBuffer2);
			UINT stride = sizeof(MYVERTEX);
			UINT of = 0;
			myContext->IASetVertexBuffers(0, 1, &skyBoxBuff, &stride, &of);
			myContext->IASetIndexBuffer(SBIBuff, DXGI_FORMAT_R32_UINT, 0);

			myContext->VSSetShader(vShader, NULL, 0);
			myContext->PSSetShader(SBpSHader, NULL, 0);
			myContext->PSSetShaderResources(0, 1, &SRVs[2]);
			myContext->PSSetSamplers(0, 1, &texSampler);

			myContext->IASetInputLayout(IL);

			myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			myContext->DrawIndexed(36, 0, 0);

			//DRAW PAINT SKYBOX
			myContext->ClearDepthStencilView(paintDSV, D3D11_CLEAR_DEPTH, 1, 0);
			myContext->OMSetRenderTargets(1, &targets[1], paintDSV);
			myContext->ClearRenderTargetView(paintRTV, d_green);
			toShader.worldMat = PAINTSKYWORLD;
			toShader.viewMat = pVIEWMATRIX;
			toShader.projMat = pPROJECTIONMATRIX;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->DrawIndexed(36, 0, 0);

			myContext->OMSetRenderTargets(1, &targets[0], myDepthStencilView);
			//

			myContext->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0); //RECLEAR depth buffer

			myContext->VSSetShader(IvShader, NULL, 0);

			toShader.worldMat = WORLDMATRIX;
			toShader.viewMat = VIEWMATRIX;
			toShader.projMat = PROJECTIONMATRIX;

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

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

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(cLightConBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &cLight, sizeof(cLight));
			myContext->Unmap(cLightConBuff, 0);

			myContext->PSSetConstantBuffers(2, 1, &cLightConBuff);

			ID3D11Buffer * floorBuffs[2];
			floorBuffs[0] = vertBuffer;
			floorBuffs[1] = floorInstanceBuff;

			myContext->IASetInputLayout(InstancingIL);

			UINT strides[2];
			strides[0] = stride;
			strides[1] = sizeof(XMFLOAT4);

			UINT offsets[2];
			offsets[0] = 0;
			offsets[1] = 0;
			
			
			myContext->IASetVertexBuffers(0, 2, floorBuffs, strides, offsets);
			

			myContext->PSSetShader(pShader, NULL, 0);

			myContext->PSSetShaderResources(0, 1, &SRVs[0]);
			
			myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
			myContext->DrawInstanced(6, 25, 0, 0);

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
			myContext->VSSetShader(WAVEvSHader, NULL, 0);
			myContext->PSSetShader(pShader, NULL, 0);

			toShader.worldMat = WOLFWORLD;

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &WolfVertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(WolfIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			//myContext->PSSetShaderResources(0, 1, &SRVs[1]);

			//myContext->DrawIndexed(6114, 0, 0);

			//DRAW ISLAND
			myContext->VSSetShader(vShader, NULL, 0);
			toShader.worldMat = ISLANDWORLD;

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &IslandVertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(IslandIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &SRVs[5]);

			myContext->DrawIndexed(IslandVertCount, 0, 0);

			//DRAW CAMPFIRE
			toShader.worldMat = CFWORLD;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &CFVertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(CFIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &SRVs[7]);

			myContext->DrawIndexed(CFVertCount, 0, 0);

			//DRAW EASEL
			toShader.worldMat = EASELWORLD;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &EaselVertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(EaselIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &SRVs[12]);

			myContext->DrawIndexed(EaselVertCount, 0, 0);

			//DRAW PALM1
			toShader.worldMat = PALM1WORLD;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &Palm1VertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(Palm1IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &SRVs[6]);

			myContext->DrawIndexed(Palm1VertCount, 0, 0);

			//DRAW TO TEXTUREEEEEEEE
			dirLight.light.z = -dirLight.light.z;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(lightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &dirLight, sizeof(dirLight));
			myContext->Unmap(lightConstBuff, 0);
			myContext->PSSetConstantBuffers(0, 1, &lightConstBuff);

			myContext->OMSetRenderTargets(1, &targets[1], paintDSV);
			
			toShader.worldMat = PAINTTREEWORLD;
			toShader.viewMat = pVIEWMATRIX;
			toShader.projMat = pPROJECTIONMATRIX;

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->DrawIndexed(Palm1VertCount, 0, 0);

			myContext->OMSetRenderTargets(1, &targets[0], myDepthStencilView);

			//DRAW PALM2
			dirLight.light.z = -dirLight.light.z;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(lightConstBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &dirLight, sizeof(dirLight));
			myContext->Unmap(lightConstBuff, 0);
			myContext->PSSetConstantBuffers(0, 1, &lightConstBuff);

			toShader.worldMat = PALM2WORLD;
			toShader.viewMat = VIEWMATRIX;
			toShader.projMat = PROJECTIONMATRIX;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &Palm2VertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(Palm2IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &SRVs[10]);

			myContext->DrawIndexed(Palm2VertCount, 0, 0);

			//DRAW PAINT
			toShader.worldMat = PAINTWORLD;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);
			myContext->IASetVertexBuffers(0, 1, &PaintVertBuffer, &stride, &of);
			myContext->PSSetShaderResources(0, 1, &SRVs[11]);

			myContext->Draw(PaintVertCount, 0);

			//DRAW FERN
			toShader.worldMat = FERNWORLD;

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &FernVertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(FernIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &SRVs[9]);

			myContext->DrawIndexed(FernVertCount, 0, 0);

			//send cam position
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(cPosConBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &cPos, sizeof(cPos));
			myContext->Unmap(cPosConBuff, 0);
			myContext->PSSetConstantBuffers(3, 1, &cPosConBuff);

			//POKEMON TIIIIME
			if (pkmn) {
				float h = cPos.spec.x;
				cPos.spec.x = 8;

				ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
				myContext->Map(cPosConBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
				memcpy(mapResource.pData, &cPos, sizeof(cPos));
				myContext->Unmap(cPosConBuff, 0);

				myContext->PSSetShader(SpecularpShader, NULL, 0);

				//KINGLER
				toShader.worldMat = KINGLERWORLD;
				ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
				myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
				memcpy(mapResource.pData, &toShader, sizeof(toShader));
				myContext->Unmap(vertBuffer2, 0);

				myContext->IASetVertexBuffers(0, 1, &KinglerVertBuffer, &stride, &of);
				myContext->IASetIndexBuffer(KinglerIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
				myContext->PSSetShaderResources(0, 1, &SRVs[13]);

				myContext->DrawIndexed(KinglerVertCount, 0, 0);

				//OMASTAR
				toShader.worldMat = OMASTARWORLD;
				ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
				myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
				memcpy(mapResource.pData, &toShader, sizeof(toShader));
				myContext->Unmap(vertBuffer2, 0);

				myContext->IASetVertexBuffers(0, 1, &OmastarVertBuffer, &stride, &of);
				myContext->IASetIndexBuffer(OmastarIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
				myContext->PSSetShaderResources(0, 1, &SRVs[14]);

				myContext->DrawIndexed(OmastarVertCount, 0, 0);

				//EXEGGUTOR
				toShader.worldMat = EXEGGWORLD;
				ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
				myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
				memcpy(mapResource.pData, &toShader, sizeof(toShader));
				myContext->Unmap(vertBuffer2, 0);

				myContext->IASetVertexBuffers(0, 1, &ExeggVertBuffer, &stride, &of);
				myContext->IASetIndexBuffer(ExeggIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
				myContext->PSSetShaderResources(0, 1, &SRVs[16]);

				myContext->DrawIndexed(ExeggVertCount, 0, 0);

				//SEADRA
				XMFLOAT4X4 temp;
				XMMATRIX mth = XMLoadFloat4x4(&SEADRAWORLD);
				XMStoreFloat4x4(&temp, XMMatrixTranslation(0, (sin(KINGLERWORLD._43 + (waveOffset.offst.x - 2.0f)) / 8.0f), 0) * mth);
				toShader.worldMat = temp;
				ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
				myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
				memcpy(mapResource.pData, &toShader, sizeof(toShader));
				myContext->Unmap(vertBuffer2, 0);

				myContext->IASetVertexBuffers(0, 1, &SeadraVertBuffer, &stride, &of);
				myContext->IASetIndexBuffer(SeadraIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
				myContext->PSSetShaderResources(0, 1, &SRVs[15]);

				myContext->DrawIndexed(SeadraVertCount, 0, 0);

				//DRAW TIE
				toShader.worldMat = TIEWORLD;

				ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
				myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
				memcpy(mapResource.pData, &toShader, sizeof(toShader));
				myContext->Unmap(vertBuffer2, 0);

				myContext->IASetVertexBuffers(0, 1, &TieVertBuffer, &stride, &of);
				myContext->IASetIndexBuffer(TieIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
				myContext->PSSetShaderResources(0, 1, &SRVs[3]);

				myContext->DrawIndexed(tieVertCount, 0, 0);

				myContext->PSSetShader(pShader, NULL, 0);
				cPos.spec.x = h;

				ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
				myContext->Map(cPosConBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
				memcpy(mapResource.pData, &cPos, sizeof(cPos));
				myContext->Unmap(cPosConBuff, 0);
			}
			//GEOMETRY STUFF
			myContext->VSSetShader(GEOvShader, NULL, 0);
			myContext->GSSetShader(gShader, NULL, 0);

			toShader.worldMat = GEOWORLD;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &GeoBuff, &stride, &of);
			myContext->GSSetConstantBuffers(0, 1, &vertBuffer2);

			myContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

			//myContext->Draw(1, 0);

			myContext->GSSetShader(NULL, NULL, 0);
			//myContext->VSSetShader(vShader, NULL, 0);

			/*toShader.worldMat = SPRLWORLD;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);*/

			//myContext->IASetVertexBuffers(0, 1, &sprlVBuff, &stride, &of);

			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(WVOffsetConBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &waveOffset, sizeof(waveOffset));
			myContext->Unmap(WVOffsetConBuff, 0);

			myContext->VSSetConstantBuffers(1, 1, &WVOffsetConBuff);
			
			myContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
			//myContext->Draw(sprlVCount, 0);

			myContext->VSSetShader(WAVEvSHader, NULL, 0);
			myContext->IASetInputLayout(IL);

			toShader.worldMat = FISHWORLD;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &FishVertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(FishIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &SRVs[8]); //srv 8

			myContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			myContext->DrawIndexed(FishVertCount, 0, 0);

			myContext->PSSetShader(OceanpShader, NULL, 0);

			toShader.worldMat = PLANEWORLD;
			ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			myContext->Map(vertBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
			memcpy(mapResource.pData, &toShader, sizeof(toShader));
			myContext->Unmap(vertBuffer2, 0);

			myContext->IASetVertexBuffers(0, 1, &OceanVertBuffer, &stride, &of);
			myContext->IASetIndexBuffer(OceanIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &SRVs[4]); //srv4

			myContext->DrawIndexed(oceanVertCount, 0, 0);

			// Present Backbuffer using Swapchain object
			// Framerate is currently unlocked, we suggest "MSI Afterburner" to track your current FPS and memory usage.
			mySwapChain->Present(0, 0);

			// Free any temp DX handles aquired this frame
			myRenderTargetView->Release();
		}
	}
}