#include "Shader.h"

Shader::~Shader() {

	// 释放每个常量缓冲区对应结构体的内存
	for (int i = 0; i < 15;i++) {
		if (constantBufferDatas[i]) {
			_aligned_free(constantBufferDatas[i]);
			// 置空,避免野指针
			constantBufferDatas[i] = nullptr;
		}
	}
}

Shader::Shader(ID3D11Device* d3dDevice, const WCHAR* cvsPath, const WCHAR* cpsPath , const WCHAR* cgsPath ){

	// 初始化
	memset(isDirty,false,sizeof(isDirty));
	memset(constantBufferDatas,NULL,sizeof(constantBufferDatas));
	memset(cbDescMap,NULL,sizeof(cbDescMap));
	memset(constantBuffers,NULL,sizeof(constantBuffers));
	memset(cb2shaderType,NULL,sizeof(cb2shaderType));
	memset(srv2shaderType,NULL,sizeof(srv2shaderType));
	memset(samplerState2shaderType,NULL,sizeof(samplerState2shaderType));

	if (cvsPath) {
		ID3DBlob* vsBuffer = LoadCompiledShaderFromFile(cvsPath);
		// 根据已编译内容创建着色器
		HR(d3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, vertexShader.GetAddressOf()));
		// 根据已编译内容进行反射,获得常量缓冲区和着色器变量信息
		ReflectShader(vsBuffer, ShaderType::VertexShader,d3dDevice);
	}
	
	if (cpsPath) {
		ID3DBlob* psBuffer = LoadCompiledShaderFromFile(cpsPath);
		// 根据已编译内容创建着色器
		HR(d3dDevice->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, pixelShader.GetAddressOf()));
		// 根据已编译内容进行反射,获得常量缓冲区和着色器变量信息
		ReflectShader(psBuffer, ShaderType::PixelShader,d3dDevice);

	}

	if (cgsPath) {
		ID3DBlob* gsBuffer = LoadCompiledShaderFromFile(cgsPath);
		// 根据已编译内容创建着色器
		HR(d3dDevice->CreateGeometryShader(gsBuffer->GetBufferPointer(), gsBuffer->GetBufferSize(), 0, geometryShader.GetAddressOf()));
		// 根据已编译内容进行反射,获得常量缓冲区和着色器变量信息
		ReflectShader(gsBuffer, ShaderType::GeometryShader,d3dDevice);
	}
	
}

Shader::Shader(const std::wstring& vsPath, const std::wstring& cvsPath, const std::wstring& psPath, const std::wstring& cpsPath, const std::wstring& gsPath, const std::wstring& cgsPath, ID3D11Device* d3dDevice) {

}

void Shader::ReflectShader(ID3DBlob* shaderBuffer,ShaderType shaderType,ID3D11Device* d3dDevice) {
	ComPtr<ID3D11ShaderReflection> shaderReflection;
	D3DReflect(shaderBuffer->GetBufferPointer(),shaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(shaderReflection.GetAddressOf()));
	
	// 获得该Shader的详细信息
	D3D11_SHADER_DESC shaderDesc;
	shaderReflection->GetDesc(&shaderDesc);		

	// 特别的，对于顶点着色器，需要初始化该Shader对应的输入布局
	if (shaderType == VertexShader) {
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,36,D3D11_INPUT_PER_VERTEX_DATA,0}
		};
		//std::vector<D3D11_SIGNATURE_PARAMETER_DESC> parameterDescs(shaderDesc.InputParameters);
		//for (uint i = 0; i < shaderDesc.InputParameters; i++) {
		//	shaderReflection->GetInputParameterDesc(i, &parameterDescs[i]);

		//	// 无视实例化的语义(这个语义可以不用包括在输入布局中)
		//	if (parameterDescs[i].SemanticName == "SV_InstanceID") continue;

		//	if(i==0)
		//		inputElements[i] = CreateInputElementDesc(
		//			parameterDescs[i].SemanticName,
		//			parameterDescs[i].SemanticIndex,
		//			InputElementMappedData::InputElementSemanticName2Format[parameterDescs[i].SemanticName],
		//			0,
		//			0,
		//			D3D11_INPUT_PER_VERTEX_DATA, 0);
		//	else 
		//		inputElements[i] = CreateInputElementDesc(
		//			parameterDescs[i].SemanticName,
		//			parameterDescs[i].SemanticIndex,
		//			InputElementMappedData::InputElementSemanticName2Format[parameterDescs[i].SemanticName],
		//			0,
		//			inputElements[i-1].AlignedByteOffset+ InputElementMappedData::InputElementFormat2Offset[inputElements[i-1].Format],
		//			D3D11_INPUT_PER_VERTEX_DATA, 0);
		//	
		//}
		// 创建输入布局
		HR(d3dDevice->CreateInputLayout(
			&inputElements[0],
			inputElements.size(),
			shaderBuffer->GetBufferPointer(),shaderBuffer->GetBufferSize(),
			inputLayout.GetAddressOf()));
	}

	for (uint i = 0; i < shaderDesc.BoundResources;i++) {

		// 获得绑定点的详细信息（用于获得目标常量缓冲区的输入槽）
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		shaderReflection->GetResourceBindingDesc(i, &bindDesc);
		
		if (bindDesc.Type == D3D_SIT_SAMPLER) {		

			#pragma region 对SamplerState进行反射,获得SampleState对应着色器情况
			uint bindPoint = bindDesc.BindPoint;

			if (samplerState2shaderType[bindPoint] == NULL) {
				samplerState2shaderType[bindPoint] = shaderType;
			} else {
				samplerState2shaderType[bindPoint] = (ShaderType)(samplerState2shaderType[bindPoint] | shaderType);
			}
			samplerStateName2slot[bindDesc.Name] = bindPoint;
			#pragma endregion

		} else if(bindDesc.Type == D3D_SIT_TEXTURE){

			#pragma region 对Texture进行反射
			uint bindPoint = bindDesc.BindPoint;

			if (srv2shaderType[bindPoint] == NULL) {
				srv2shaderType[bindPoint] = shaderType;
			} else {
				srv2shaderType[bindPoint] = (ShaderType)(srv2shaderType[bindPoint] | shaderType);
			}
			srvName2slot[bindDesc.Name] = bindPoint;
			#pragma endregion

		} else if (bindDesc.Type == D3D_SIT_CBUFFER) {
		
			#pragma region 对常量缓冲区进行反射
			// 获得对应常量缓冲区的描述
			ID3D11ShaderReflectionConstantBuffer* cb = shaderReflection->GetConstantBufferByName(bindDesc.Name);
			D3D11_SHADER_BUFFER_DESC cbDesc;
			cb->GetDesc(&cbDesc);

			uint bindPoint = bindDesc.BindPoint;

			// 设置该常量缓冲区对应的详细信息映射
			cbDescMap[bindPoint] = cbDesc;


			// 设置该常量缓冲区对应的着色器(即该常量缓冲区所绑定的着色器)
			if (cb2shaderType[bindPoint] == NULL) {
				cb2shaderType[bindPoint] = shaderType;
			} else {
				cb2shaderType[bindPoint] = (ShaderType)(cb2shaderType[bindPoint] | shaderType);
			}

			// 为该常量缓冲区申请一块内存
			if (!constantBufferDatas[bindPoint]) {
				constantBufferDatas[bindPoint] = _aligned_malloc(cbDesc.Size, 16);
				// 初始化
				memset(constantBufferDatas[bindPoint], 0, cbDesc.Size);
			}

			// 为该常量缓冲区创建一块Id3d11Buffer
			if (!constantBuffers[bindPoint])
				HR(CreateBuffer(
					D3D11_USAGE_DYNAMIC,
					cbDesc.Size,
					D3D11_BIND_CONSTANT_BUFFER,
					D3D11_CPU_ACCESS_WRITE,
					0,
					cbDesc.Size,
					nullptr,
					d3dDevice,
					constantBuffers[bindPoint].GetAddressOf()
				));

			varibleArray[bindPoint].resize(cbDesc.Variables);
			// 遍历该常量缓冲区下的着色器变量
			for (uint j = 0; j < cbDesc.Variables; j++) {
				// 获得该着色器变量的详细信息
				ID3D11ShaderReflectionVariable* sVariable = cb->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC vDesc;
				sVariable->GetDesc(&vDesc);
				
				std::string svName = vDesc.Name;

				// 设置该着色器变量的具体位置信息
				varibleMap[svName] = ShaderVaribleLocation(bindPoint, j);

				// 设置该着色器变量在常量缓冲区中的下标所对应的详细信息
				varibleArray[bindPoint][j] = vDesc;
			}

			#pragma endregion

		} else if(bindDesc.Type == D3D_SIT_STRUCTURED){
			
			#pragma region 对自定义数据结构进行反射
			uint bindPoint = bindDesc.BindPoint;

			if (srv2shaderType[bindPoint] == NULL) {
				srv2shaderType[bindPoint] = shaderType;
			} else {
				srv2shaderType[bindPoint] = (ShaderType)(srv2shaderType[bindPoint] | shaderType);
			}
			#pragma endregion

		}
		
	}
}


void Shader::SetShaderResource(uint slot, ID3D11ShaderResourceView* const * shaderResource,ID3D11DeviceContext* const deviceContext) {
	if ( (srv2shaderType[slot] | VertexShader) == srv2shaderType[slot]) {
		deviceContext->VSSetShaderResources(slot,1,shaderResource);
	}
	if ( (srv2shaderType[slot] | PixelShader) == srv2shaderType[slot]) {
		deviceContext->PSSetShaderResources(slot,1,shaderResource);
	}
	if ( (srv2shaderType[slot] | GeometryShader) == srv2shaderType[slot]) {
		deviceContext->GSSetShaderResources(slot,1,shaderResource);
	}	
	
}

void Shader::SetSamplerState(uint slot, ID3D11SamplerState* const * samplerState, ID3D11DeviceContext* const deviceContext) {
	if ((samplerState2shaderType[slot] | VertexShader) == samplerState2shaderType[slot]) {
		deviceContext->VSSetSamplers(slot,1,samplerState);
	}
	if ((samplerState2shaderType[slot] | PixelShader) == samplerState2shaderType[slot]) {
		deviceContext->PSSetSamplers(slot, 1, samplerState);
	}
	if ((samplerState2shaderType[slot] | GeometryShader) == samplerState2shaderType[slot]) {
		deviceContext->GSSetSamplers(slot, 1, samplerState);
	}
}

void Shader::SetRasterState(ID3D11RasterizerState* rasterState) {
	this->rasterState = rasterState;
}
void Shader::SetDepthStencilState(ID3D11DepthStencilState* depthStencilState, uint stencilRef) {
	this->depthStencilState = depthStencilState;
	this->stencilRef = stencilRef;
}
void Shader::SetBlendState(ID3D11BlendState* blendState) {
	this->blendState = blendState;
}

void Shader::Use(ID3D11DeviceContext* context) {
	// 第一步,把该有的着色器都绑定到渲染管线上
	if (vertexShader) context->VSSetShader(vertexShader.Get(),0,0);
	if (pixelShader) context->PSSetShader(pixelShader.Get(),0,0);
	if (geometryShader) context->GSSetShader(geometryShader.Get(),0,0);

	// 第二步,设置常量缓冲区
	for (int i = 0; i < 15; i++) {
		// 如果该槽位的数据被CPU更新过,
		// 那么就需要将该数据同样更新到GPU中
		if (isDirty[i]) {
			// 复原
			isDirty[i] = false;

			// 获取该槽位对应的常量缓冲区
			std::string cbName = slot2CbNameMap[i];
			void* bufferData = constantBufferDatas[i];
			ComPtr<ID3D11Buffer> buffer = constantBuffers[i];

			D3D11_MAPPED_SUBRESOURCE mappedData;
			// 将数据映射到GPU中
			HR(context->Map(buffer.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mappedData));
			memcpy(mappedData.pData,bufferData, cbDescMap[i].Size);
			context->Unmap(buffer.Get(),0);

			// 根据该槽位常量缓冲区对应的着色器,对相应的着色器进行设置常量缓冲区
			if ((cb2shaderType[i] | VertexShader) == cb2shaderType[i]) 
				context->VSSetConstantBuffers(i,1,buffer.GetAddressOf());
			
			if ((cb2shaderType[i] | PixelShader) == cb2shaderType[i])
				context->PSSetConstantBuffers(i,1,buffer.GetAddressOf());
			
			if ((cb2shaderType[i] | GeometryShader) == cb2shaderType[i])
				context->GSSetConstantBuffers(i,1,buffer.GetAddressOf());
		}
	}
	
	// 第三步，如果存在自定义的混合状态或光栅化状态或深度测试状态，进行设置
	if (blendState != nullptr) {
		const float BlendFactor[] = { 0,0,0,0 };
		context->OMSetBlendState(blendState.Get(), BlendFactor, 0xffffffff);
	}
	if (rasterState != nullptr) {
		context->RSSetState(rasterState.Get());
	}
	if (depthStencilState != nullptr) {
		context->OMSetDepthStencilState(depthStencilState.Get(), stencilRef);
	}

	// 第四步，设置顶点输入布局	
	context->IASetInputLayout(inputLayout.Get());
}

const Shader::ShaderVaribleLocation* Shader::GetVaribleLocation(const std::string& name) {
	if (varibleMap.find(name) == varibleMap.end())
		return nullptr;

	return &varibleMap[name];
}

const uint Shader::GetSRVSlot(const std::string& name) {
	if (srvName2slot.find(name) == srvName2slot.end())
		return -1;
	return srvName2slot[name];
}

const uint Shader::GetSamplerStateSlot(const std::string& name) {
	if (samplerStateName2slot.find(name) == samplerStateName2slot.end()) return -1;
	return samplerStateName2slot[name];
}