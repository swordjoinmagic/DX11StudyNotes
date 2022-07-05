#include "D3DUtils.h"
#include <D3Dcompiler.h>
#include "MathF.h"

ID3DBlob* LoadCompiledShaderFromFile(const std::wstring& fileName) {
	std::ifstream fin(fileName, std::ios::binary);
	fin.seekg(0, std::ios_base::end);
	std::ifstream::pos_type size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);

	ID3DBlob* blob;
	HR(D3DCreateBlob(size, &blob));
	
	fin.read((char*)blob->GetBufferPointer(), size);
	fin.close();

	return blob;
}

std::vector<char> LoadCompiledEffectFile(const std::wstring& fileName,int& size) {
	std::ifstream fin(fileName,std::ios::binary);
	fin.seekg(0,std::ios_base::end);
	size = (int)fin.tellg();
	fin.seekg(0,std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0],size);
	fin.close();

	return compiledShader;
}

//TODO~
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
) {
	D3D11_BUFFER_DESC bufferDesc = {
		byteWidth,
		usage,		
		bindFlag,
		cpuAccessFlags,
		miscFlags,
		structureByteStride
	};

	D3D11_SUBRESOURCE_DATA bufferData;
	bufferData.pSysMem = data;
	
	if(data)
		return d3dDevice->CreateBuffer(&bufferDesc,&bufferData,buffer);
	else {
		return d3dDevice->CreateBuffer(&bufferDesc, 0, buffer);
	}
}

//TODO~
D3D11_INPUT_ELEMENT_DESC CreateInputElementDesc(
	LPCSTR SemanticName,
	uint SemanticIndex,
	DXGI_FORMAT Format,
	uint InputSlot,
	uint AlignedByteOffset,
	D3D11_INPUT_CLASSIFICATION InputSlotClass,
	uint InstanceDataStepRate) {
	return { SemanticName ,SemanticIndex ,Format,InputSlot,AlignedByteOffset,InputSlotClass,InstanceDataStepRate };
}


//TODO~
// 输入布局中,语义与DXGI_Format一一对应的map
std::map<std::string, DXGI_FORMAT> InputElementMappedData::InputElementSemanticName2Format = {
	{"POSITION",DXGI_FORMAT_R32G32B32_FLOAT},
	{"NORMAL",DXGI_FORMAT_R32G32B32_FLOAT},
	{"TANGENT",DXGI_FORMAT_R32G32B32_FLOAT},
	{"COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT},
	{"TEXCOORD",DXGI_FORMAT_R32G32_FLOAT}
};
// format与offset一一对应
std::map<DXGI_FORMAT, uint> InputElementMappedData::InputElementFormat2Offset = {
	{DXGI_FORMAT_R32G32B32A32_FLOAT,16},
	{DXGI_FORMAT_R32G32B32_FLOAT,12},
	{DXGI_FORMAT_R32G32B32_FLOAT,12},
	{DXGI_FORMAT_R32G32B32_FLOAT,12},
	{DXGI_FORMAT_R32G32_FLOAT,8}
};