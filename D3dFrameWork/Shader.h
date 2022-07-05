#pragma once
#include "D3DUtils.h"
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <map>
#include <unordered_map>
#include <string>

class Shader {
public:
	// 用于描述某个着色器变量所在的具体位置
	struct ShaderVaribleLocation {
		// 其所在常量缓冲区的槽位
		int slot;
		// 其在常量缓冲区中的局部下标
		int index;
		ShaderVaribleLocation() { slot = 0; index = 0; }
		ShaderVaribleLocation(int _slot, int _index) :slot(_slot), index(_index) {}
	};
private:
	// 用于表示着色器类型的枚举
	// 判断时,只需要将value | judgeType == value,即可知道value是否包含judeType
	enum ShaderType {
		VertexShader = 1,
		PixelShader = 2,
		vs_ps = 3,		// 1|2 = 3
		GeometryShader = 4,
		vs_gs = 5,		// 1|4 = 5
		ps_gs = 6,		// 2|4 = 6
		ALL				// 1|2|4 = 7
	};
private:
	// 着色器集合
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11GeometryShader> geometryShader;

	// 渲染状态集合
	ComPtr<ID3D11RasterizerState> rasterState;
	ComPtr<ID3D11BlendState> blendState;
	ComPtr<ID3D11DepthStencilState> depthStencilState;
	uint stencilRef = 0;

	// 输入布局
	ComPtr<ID3D11InputLayout> inputLayout;

	// 键为槽位,值为常量缓冲区名字
	std::string slot2CbNameMap[15];

	// 键位常量缓冲区槽位,值为常量缓冲区详细信息
	D3D11_SHADER_BUFFER_DESC cbDescMap[15];

	// 二维数组,varibleArray[i][j]表示,第i号常量缓冲区中的下标为j的着色器变量所对应的详细信息
	std::vector<D3D11_SHADER_VARIABLE_DESC> varibleArray[15];

	// 键为该着色器变量的名字，值为该变量的具体位置
	std::unordered_map<std::string, ShaderVaribleLocation> varibleMap;

	// 键为常量缓冲区的槽位，值为申请的该常量缓冲区的一块内存的指针，
	// 大小由该常量缓冲区决定
	void *constantBufferDatas[15];

	// 键位常量缓冲区的槽位,值为GPU中的常量缓冲区,为id3d11Buffer类型,
	// 主要用于传输数据到GPU
	ComPtr<ID3D11Buffer> constantBuffers[15];

	// 表示各个常量缓冲区绑定的着色器,键位常量缓冲区绑定的槽位,值为该槽位所对应的着色器类型
	ShaderType cb2shaderType[15];
	// 表示各个着色器资源视图对应的着色器类型,键为槽位
	ShaderType srv2shaderType[15];
	// 表示各个着色器资源视图名称对应的槽位
	std::unordered_map<std::string,uint> srvName2slot;
	// 表示各个采样状态对应的着色器类型，键为槽位
	ShaderType samplerState2shaderType[15];
	// 表示各个采样状态名称对应的槽位
	std::unordered_map<std::string, uint> samplerStateName2slot;

	// 用于记录某个常量缓冲区是否被更新，下标为槽位，
	// 如isDirty[0] = true表示槽位为0的常量缓冲区被更新了
	bool isDirty[15];

public:
	// 根据已编译的着色器代码来初始化着色器,cvs - compiled vertex shader
	Shader(ID3D11Device* d3dDevice,const WCHAR* cvsPath = nullptr,const WCHAR* cpsPath = nullptr,const WCHAR* cgsPath = nullptr);
	// 根据未编译的着色器代码初始化着色器，并自动将已编译的着色器写入文件
	Shader(const std::wstring& vsPath, const std::wstring& cvsPath, const std::wstring& psPath, const std::wstring& cpsPath, const std::wstring& gsPath, const std::wstring& cgsPath, ID3D11Device* d3dDevice);
	virtual ~Shader();

	#pragma region 用于设置常量缓冲区中变量的泛型函数

	// 设置着色器变量的方法，只需要提供变量的名字，在内部自动查找该着色器变量所在内存位置，并进行相应设置
	template <class T>
	void SetValue(const std::string& name, const T& value){
		ShaderVaribleLocation location = varibleMap[name];
		SetValue<T>(location.slot,location.index,value);
	}
	template <class T>
	void SetValue(uint slot,uint varIndex,const T& value) {
		// 根据槽位与着色器变量下标,找到该着色器变量的详细描述(主要为了得到offset)
		D3D11_SHADER_VARIABLE_DESC desc = varibleArray[slot][varIndex];
		// 根据常量缓冲区槽位,找到对应的缓冲区数据
		void* memory = constantBufferDatas[slot];
		// 根据该着色器变量的offset,对常量缓冲区内存相应位置进行设置
		memcpy(reinterpret_cast<char*>(memory) + desc.StartOffset, &value, sizeof(value));
		// 设置该槽位的Dirty信息
		isDirty[slot] = true;
	}

	// 特殊的设置着色器中数组类型变量的方法
	template <class T>
	void SetArrayValue(uint slot, uint varIndex, const T* value, uint size) {
		// 根据槽位与着色器变量下标,找到该着色器变量的详细描述(主要为了得到offset)
		D3D11_SHADER_VARIABLE_DESC desc = varibleArray[slot][varIndex];
		// 根据常量缓冲区槽位,找到对应的缓冲区数据
		void* memory = constantBufferDatas[slot];
		// 根据该着色器变量的offset,对常量缓冲区内存相应位置进行设置
		memcpy(reinterpret_cast<char*>(memory) + desc.StartOffset, value, sizeof(T)*size);
		// 设置该槽位的Dirty信息
		isDirty[slot] = true;
	}
	template <class T>
	void SetArrayValue(const std::string& name, const T* value, uint size) {
		ShaderVaribleLocation location = varibleMap[name];
		SetArrayValue<T>(location.slot, location.index, value,size);
	}

	// 获得着色器变量的方法，需要注意的是返回的是只读指针,为了防止变量在外部被修改
	template <class T>
	std::shared_ptr<const T> GetValue(const std::string& name) {		
		// 找到变量的具体位置
		ShaderVaribleLocation location = varibleMap[name];
		return GetValue<T>(location.slot,location.index);
	}

	template <class T>
	std::shared_ptr<const T> GetValue(int slot,uint varIndex) {		
		// 根据槽位与着色器变量下标,找到该着色器变量的详细描述(主要为了得到offset)
		D3D11_SHADER_VARIABLE_DESC desc = varibleArray[slot][varIndex];

		// 申请内存
		void* value = malloc(desc.Size);

		// 根据常量缓冲区槽位,找到对应的缓冲区数据
		void* memory = constantBufferDatas[slot];

		memcpy(value, reinterpret_cast<char*>(memory) + desc.StartOffset,desc.Size);

		return std::shared_ptr<const T>(reinterpret_cast<T*>(value));
	}
	#pragma endregion

	#pragma region 用于绑定管线上的资源的函数(SRV,SamplerState)

	// 用于设置Texture/CubeMap/StructData等数据
	void SetShaderResource(uint slot, ID3D11ShaderResourceView* const * shaderResource, ID3D11DeviceContext* const deviceContext);
	
	void SetSamplerState(uint slot, ID3D11SamplerState* const * samplerState, ID3D11DeviceContext* const deviceContext);

	#pragma endregion
	
	
	// 用于设置渲染状态
	void SetRasterState(ID3D11RasterizerState* rasterState);
	void SetDepthStencilState(ID3D11DepthStencilState* depthStencilState, uint stencilRef);
	void SetBlendState(ID3D11BlendState* blendState);

	// 用于获取着色器常量缓冲区/着色器变量等信息
	void ReflectShader(ID3DBlob* shaderBuffer,ShaderType shaderType, ID3D11Device* d3dDevice);

	// 在绘制之前调用，自动设置上下文（如各种SSetShader和常量缓冲区）
	void Use(ID3D11DeviceContext* context);

	// 获得某个着色器变量在常量缓冲区中的位置
	const ShaderVaribleLocation* GetVaribleLocation(const std::string& name);
	// 获得某个着色器资源所在的槽口,-1表示未找到资源
	const uint GetSRVSlot(const std::string& name);

	// 根据采样状态名称,获得该采样状态对应的槽口,-1表示未找到资源
	const uint GetSamplerStateSlot(const std::string& name);
};