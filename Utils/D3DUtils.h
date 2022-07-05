#pragma once

#define DEBUG

#include <d3d11.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <DirectXMath.h>
#include "DxTrace.h"
#include <wrl/client.h>

using namespace DirectX;

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

typedef XMFLOAT2 float2;
typedef XMFLOAT3 float3;
typedef XMFLOAT4 float4;
typedef XMFLOAT4X4 float4x4;
typedef XMFLOAT4 Color;
typedef UINT uint;


#define VK_KEY_A 0x41
#define VK_KEY_B 0x42
#define VK_KEY_C 0x43
#define VK_KEY_D 0x44
#define VK_KEY_E 0x45
#define VK_KEY_F 0x46
#define VK_KEY_G 0x47
#define VK_KEY_H 0x48
#define VK_KEY_I 0x49
#define VK_KEY_J 0x4A
#define VK_KEY_K 0x4B
#define VK_KEY_L 0x4C
#define VK_KEY_M 0x4D
#define VK_KEY_N 0x4E
#define VK_KEY_O 0x4F
#define VK_KEY_P 0x50
#define VK_KEY_Q 0x51
#define VK_KEY_R 0x52
#define VK_KEY_S 0x53
#define VK_KEY_T 0x54
#define VK_KEY_U 0x55
#define VK_KEY_V 0x56
#define VK_KEY_W 0x57
#define VK_KEY_X 0x58
#define VK_KEY_Y 0x59
#define VK_KEY_Z 0x5A

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }


namespace Colors {
	XMGLOBALCONST XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

	XMGLOBALCONST XMVECTORF32 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
	XMGLOBALCONST XMVECTORF32 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
};

#pragma region Shader编译相关
ID3DBlob* LoadCompiledShaderFromFile(const std::wstring& fileName);
std::vector<char> LoadCompiledEffectFile(const std::wstring& fileName, int& size);
#pragma endregion


// 简化申请缓冲流程
HRESULT CreateBuffer(
	D3D11_USAGE usage,
	uint byteWidth,
	uint bindFlag,
	uint cpuAccessFlags,
	uint miscFlags,
	uint structureByteStride,
	const void *data,
	ID3D11Device* d3dDevice,
	ID3D11Buffer** buffer
);

// 简化创建Input_Element_Desc结构体的流程
D3D11_INPUT_ELEMENT_DESC CreateInputElementDesc(
	LPCSTR SemanticName,
	uint SemanticIndex,
	DXGI_FORMAT Format,
	uint InputSlot,
	uint AlignedByteOffset,
	D3D11_INPUT_CLASSIFICATION InputSlotClass,
	uint InstanceDataStepRate);

class InputElementMappedData {
public:
	// 输入布局中,语义与DXGI_Format一一对应的map
	static std::map<std::string, DXGI_FORMAT> InputElementSemanticName2Format;
	// format与offset一一对应
	static std::map<DXGI_FORMAT, uint> InputElementFormat2Offset;
};

//// 输入布局中,语义与DXGI_Format一一对应的map
//static const std::map<std::string, DXGI_FORMAT> InputElementSemanticName2Format = {
//	{"POSITION",DXGI_FORMAT_R32G32B32_FLOAT},
//	{"NORMAL",DXGI_FORMAT_R32G32B32_FLOAT},
//	{"TANGENT",DXGI_FORMAT_R32G32B32_FLOAT},
//	{"TEXCOORD",DXGI_FORMAT_R32G32_FLOAT}
//};
//// format与offset一一对应
//static const std::map<DXGI_FORMAT, uint> InputElementFormat2Offset = {
//	{DXGI_FORMAT_R32G32B32_FLOAT,12},
//	{DXGI_FORMAT_R32G32B32_FLOAT,12},
//	{DXGI_FORMAT_R32G32B32_FLOAT,12},
//	{DXGI_FORMAT_R32G32_FLOAT,8}
//};