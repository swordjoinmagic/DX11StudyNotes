#pragma once
#include "D3DUtils.h"
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <map>
#include <unordered_map>
#include <string>

class Shader {
public:
	// ��������ĳ����ɫ���������ڵľ���λ��
	struct ShaderVaribleLocation {
		// �����ڳ����������Ĳ�λ
		int slot;
		// ���ڳ����������еľֲ��±�
		int index;
		ShaderVaribleLocation() { slot = 0; index = 0; }
		ShaderVaribleLocation(int _slot, int _index) :slot(_slot), index(_index) {}
	};
private:
	// ���ڱ�ʾ��ɫ�����͵�ö��
	// �ж�ʱ,ֻ��Ҫ��value | judgeType == value,����֪��value�Ƿ����judeType
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
	// ��ɫ������
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11GeometryShader> geometryShader;

	// ��Ⱦ״̬����
	ComPtr<ID3D11RasterizerState> rasterState;
	ComPtr<ID3D11BlendState> blendState;
	ComPtr<ID3D11DepthStencilState> depthStencilState;
	uint stencilRef = 0;

	// ���벼��
	ComPtr<ID3D11InputLayout> inputLayout;

	// ��Ϊ��λ,ֵΪ��������������
	std::string slot2CbNameMap[15];

	// ��λ������������λ,ֵΪ������������ϸ��Ϣ
	D3D11_SHADER_BUFFER_DESC cbDescMap[15];

	// ��ά����,varibleArray[i][j]��ʾ,��i�ų����������е��±�Ϊj����ɫ����������Ӧ����ϸ��Ϣ
	std::vector<D3D11_SHADER_VARIABLE_DESC> varibleArray[15];

	// ��Ϊ����ɫ�����������֣�ֵΪ�ñ����ľ���λ��
	std::unordered_map<std::string, ShaderVaribleLocation> varibleMap;

	// ��Ϊ�����������Ĳ�λ��ֵΪ����ĸó�����������һ���ڴ��ָ�룬
	// ��С�ɸó�������������
	void *constantBufferDatas[15];

	// ��λ�����������Ĳ�λ,ֵΪGPU�еĳ���������,Ϊid3d11Buffer����,
	// ��Ҫ���ڴ������ݵ�GPU
	ComPtr<ID3D11Buffer> constantBuffers[15];

	// ��ʾ���������������󶨵���ɫ��,��λ�����������󶨵Ĳ�λ,ֵΪ�ò�λ����Ӧ����ɫ������
	ShaderType cb2shaderType[15];
	// ��ʾ������ɫ����Դ��ͼ��Ӧ����ɫ������,��Ϊ��λ
	ShaderType srv2shaderType[15];
	// ��ʾ������ɫ����Դ��ͼ���ƶ�Ӧ�Ĳ�λ
	std::unordered_map<std::string,uint> srvName2slot;
	// ��ʾ��������״̬��Ӧ����ɫ�����ͣ���Ϊ��λ
	ShaderType samplerState2shaderType[15];
	// ��ʾ��������״̬���ƶ�Ӧ�Ĳ�λ
	std::unordered_map<std::string, uint> samplerStateName2slot;

	// ���ڼ�¼ĳ�������������Ƿ񱻸��£��±�Ϊ��λ��
	// ��isDirty[0] = true��ʾ��λΪ0�ĳ�����������������
	bool isDirty[15];

public:
	// �����ѱ������ɫ����������ʼ����ɫ��,cvs - compiled vertex shader
	Shader(ID3D11Device* d3dDevice,const WCHAR* cvsPath = nullptr,const WCHAR* cpsPath = nullptr,const WCHAR* cgsPath = nullptr);
	// ����δ�������ɫ�������ʼ����ɫ�������Զ����ѱ������ɫ��д���ļ�
	Shader(const std::wstring& vsPath, const std::wstring& cvsPath, const std::wstring& psPath, const std::wstring& cpsPath, const std::wstring& gsPath, const std::wstring& cgsPath, ID3D11Device* d3dDevice);
	virtual ~Shader();

	#pragma region �������ó����������б����ķ��ͺ���

	// ������ɫ�������ķ�����ֻ��Ҫ�ṩ���������֣����ڲ��Զ����Ҹ���ɫ�����������ڴ�λ�ã���������Ӧ����
	template <class T>
	void SetValue(const std::string& name, const T& value){
		ShaderVaribleLocation location = varibleMap[name];
		SetValue<T>(location.slot,location.index,value);
	}
	template <class T>
	void SetValue(uint slot,uint varIndex,const T& value) {
		// ���ݲ�λ����ɫ�������±�,�ҵ�����ɫ����������ϸ����(��ҪΪ�˵õ�offset)
		D3D11_SHADER_VARIABLE_DESC desc = varibleArray[slot][varIndex];
		// ���ݳ�����������λ,�ҵ���Ӧ�Ļ���������
		void* memory = constantBufferDatas[slot];
		// ���ݸ���ɫ��������offset,�Գ����������ڴ���Ӧλ�ý�������
		memcpy(reinterpret_cast<char*>(memory) + desc.StartOffset, &value, sizeof(value));
		// ���øò�λ��Dirty��Ϣ
		isDirty[slot] = true;
	}

	// �����������ɫ�����������ͱ����ķ���
	template <class T>
	void SetArrayValue(uint slot, uint varIndex, const T* value, uint size) {
		// ���ݲ�λ����ɫ�������±�,�ҵ�����ɫ����������ϸ����(��ҪΪ�˵õ�offset)
		D3D11_SHADER_VARIABLE_DESC desc = varibleArray[slot][varIndex];
		// ���ݳ�����������λ,�ҵ���Ӧ�Ļ���������
		void* memory = constantBufferDatas[slot];
		// ���ݸ���ɫ��������offset,�Գ����������ڴ���Ӧλ�ý�������
		memcpy(reinterpret_cast<char*>(memory) + desc.StartOffset, value, sizeof(T)*size);
		// ���øò�λ��Dirty��Ϣ
		isDirty[slot] = true;
	}
	template <class T>
	void SetArrayValue(const std::string& name, const T* value, uint size) {
		ShaderVaribleLocation location = varibleMap[name];
		SetArrayValue<T>(location.slot, location.index, value,size);
	}

	// �����ɫ�������ķ�������Ҫע����Ƿ��ص���ֻ��ָ��,Ϊ�˷�ֹ�������ⲿ���޸�
	template <class T>
	std::shared_ptr<const T> GetValue(const std::string& name) {		
		// �ҵ������ľ���λ��
		ShaderVaribleLocation location = varibleMap[name];
		return GetValue<T>(location.slot,location.index);
	}

	template <class T>
	std::shared_ptr<const T> GetValue(int slot,uint varIndex) {		
		// ���ݲ�λ����ɫ�������±�,�ҵ�����ɫ����������ϸ����(��ҪΪ�˵õ�offset)
		D3D11_SHADER_VARIABLE_DESC desc = varibleArray[slot][varIndex];

		// �����ڴ�
		void* value = malloc(desc.Size);

		// ���ݳ�����������λ,�ҵ���Ӧ�Ļ���������
		void* memory = constantBufferDatas[slot];

		memcpy(value, reinterpret_cast<char*>(memory) + desc.StartOffset,desc.Size);

		return std::shared_ptr<const T>(reinterpret_cast<T*>(value));
	}
	#pragma endregion

	#pragma region ���ڰ󶨹����ϵ���Դ�ĺ���(SRV,SamplerState)

	// ��������Texture/CubeMap/StructData������
	void SetShaderResource(uint slot, ID3D11ShaderResourceView* const * shaderResource, ID3D11DeviceContext* const deviceContext);
	
	void SetSamplerState(uint slot, ID3D11SamplerState* const * samplerState, ID3D11DeviceContext* const deviceContext);

	#pragma endregion
	
	
	// ����������Ⱦ״̬
	void SetRasterState(ID3D11RasterizerState* rasterState);
	void SetDepthStencilState(ID3D11DepthStencilState* depthStencilState, uint stencilRef);
	void SetBlendState(ID3D11BlendState* blendState);

	// ���ڻ�ȡ��ɫ������������/��ɫ����������Ϣ
	void ReflectShader(ID3DBlob* shaderBuffer,ShaderType shaderType, ID3D11Device* d3dDevice);

	// �ڻ���֮ǰ���ã��Զ����������ģ������SSetShader�ͳ�����������
	void Use(ID3D11DeviceContext* context);

	// ���ĳ����ɫ�������ڳ����������е�λ��
	const ShaderVaribleLocation* GetVaribleLocation(const std::string& name);
	// ���ĳ����ɫ����Դ���ڵĲۿ�,-1��ʾδ�ҵ���Դ
	const uint GetSRVSlot(const std::string& name);

	// ���ݲ���״̬����,��øò���״̬��Ӧ�Ĳۿ�,-1��ʾδ�ҵ���Դ
	const uint GetSamplerStateSlot(const std::string& name);
};