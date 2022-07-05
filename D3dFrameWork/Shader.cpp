#include "Shader.h"

Shader::~Shader() {

	// �ͷ�ÿ��������������Ӧ�ṹ����ڴ�
	for (int i = 0; i < 15;i++) {
		if (constantBufferDatas[i]) {
			_aligned_free(constantBufferDatas[i]);
			// �ÿ�,����Ұָ��
			constantBufferDatas[i] = nullptr;
		}
	}
}

Shader::Shader(ID3D11Device* d3dDevice, const WCHAR* cvsPath, const WCHAR* cpsPath , const WCHAR* cgsPath ){

	// ��ʼ��
	memset(isDirty,false,sizeof(isDirty));
	memset(constantBufferDatas,NULL,sizeof(constantBufferDatas));
	memset(cbDescMap,NULL,sizeof(cbDescMap));
	memset(constantBuffers,NULL,sizeof(constantBuffers));
	memset(cb2shaderType,NULL,sizeof(cb2shaderType));
	memset(srv2shaderType,NULL,sizeof(srv2shaderType));
	memset(samplerState2shaderType,NULL,sizeof(samplerState2shaderType));

	if (cvsPath) {
		ID3DBlob* vsBuffer = LoadCompiledShaderFromFile(cvsPath);
		// �����ѱ������ݴ�����ɫ��
		HR(d3dDevice->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, vertexShader.GetAddressOf()));
		// �����ѱ������ݽ��з���,��ó�������������ɫ��������Ϣ
		ReflectShader(vsBuffer, ShaderType::VertexShader,d3dDevice);
	}
	
	if (cpsPath) {
		ID3DBlob* psBuffer = LoadCompiledShaderFromFile(cpsPath);
		// �����ѱ������ݴ�����ɫ��
		HR(d3dDevice->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, pixelShader.GetAddressOf()));
		// �����ѱ������ݽ��з���,��ó�������������ɫ��������Ϣ
		ReflectShader(psBuffer, ShaderType::PixelShader,d3dDevice);

	}

	if (cgsPath) {
		ID3DBlob* gsBuffer = LoadCompiledShaderFromFile(cgsPath);
		// �����ѱ������ݴ�����ɫ��
		HR(d3dDevice->CreateGeometryShader(gsBuffer->GetBufferPointer(), gsBuffer->GetBufferSize(), 0, geometryShader.GetAddressOf()));
		// �����ѱ������ݽ��з���,��ó�������������ɫ��������Ϣ
		ReflectShader(gsBuffer, ShaderType::GeometryShader,d3dDevice);
	}
	
}

Shader::Shader(const std::wstring& vsPath, const std::wstring& cvsPath, const std::wstring& psPath, const std::wstring& cpsPath, const std::wstring& gsPath, const std::wstring& cgsPath, ID3D11Device* d3dDevice) {

}

void Shader::ReflectShader(ID3DBlob* shaderBuffer,ShaderType shaderType,ID3D11Device* d3dDevice) {
	ComPtr<ID3D11ShaderReflection> shaderReflection;
	D3DReflect(shaderBuffer->GetBufferPointer(),shaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(shaderReflection.GetAddressOf()));
	
	// ��ø�Shader����ϸ��Ϣ
	D3D11_SHADER_DESC shaderDesc;
	shaderReflection->GetDesc(&shaderDesc);		

	// �ر�ģ����ڶ�����ɫ������Ҫ��ʼ����Shader��Ӧ�����벼��
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

		//	// ����ʵ����������(���������Բ��ð��������벼����)
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
		// �������벼��
		HR(d3dDevice->CreateInputLayout(
			&inputElements[0],
			inputElements.size(),
			shaderBuffer->GetBufferPointer(),shaderBuffer->GetBufferSize(),
			inputLayout.GetAddressOf()));
	}

	for (uint i = 0; i < shaderDesc.BoundResources;i++) {

		// ��ð󶨵����ϸ��Ϣ�����ڻ��Ŀ�곣��������������ۣ�
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		shaderReflection->GetResourceBindingDesc(i, &bindDesc);
		
		if (bindDesc.Type == D3D_SIT_SAMPLER) {		

			#pragma region ��SamplerState���з���,���SampleState��Ӧ��ɫ�����
			uint bindPoint = bindDesc.BindPoint;

			if (samplerState2shaderType[bindPoint] == NULL) {
				samplerState2shaderType[bindPoint] = shaderType;
			} else {
				samplerState2shaderType[bindPoint] = (ShaderType)(samplerState2shaderType[bindPoint] | shaderType);
			}
			samplerStateName2slot[bindDesc.Name] = bindPoint;
			#pragma endregion

		} else if(bindDesc.Type == D3D_SIT_TEXTURE){

			#pragma region ��Texture���з���
			uint bindPoint = bindDesc.BindPoint;

			if (srv2shaderType[bindPoint] == NULL) {
				srv2shaderType[bindPoint] = shaderType;
			} else {
				srv2shaderType[bindPoint] = (ShaderType)(srv2shaderType[bindPoint] | shaderType);
			}
			srvName2slot[bindDesc.Name] = bindPoint;
			#pragma endregion

		} else if (bindDesc.Type == D3D_SIT_CBUFFER) {
		
			#pragma region �Գ������������з���
			// ��ö�Ӧ����������������
			ID3D11ShaderReflectionConstantBuffer* cb = shaderReflection->GetConstantBufferByName(bindDesc.Name);
			D3D11_SHADER_BUFFER_DESC cbDesc;
			cb->GetDesc(&cbDesc);

			uint bindPoint = bindDesc.BindPoint;

			// ���øó�����������Ӧ����ϸ��Ϣӳ��
			cbDescMap[bindPoint] = cbDesc;


			// ���øó�����������Ӧ����ɫ��(���ó������������󶨵���ɫ��)
			if (cb2shaderType[bindPoint] == NULL) {
				cb2shaderType[bindPoint] = shaderType;
			} else {
				cb2shaderType[bindPoint] = (ShaderType)(cb2shaderType[bindPoint] | shaderType);
			}

			// Ϊ�ó�������������һ���ڴ�
			if (!constantBufferDatas[bindPoint]) {
				constantBufferDatas[bindPoint] = _aligned_malloc(cbDesc.Size, 16);
				// ��ʼ��
				memset(constantBufferDatas[bindPoint], 0, cbDesc.Size);
			}

			// Ϊ�ó�������������һ��Id3d11Buffer
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
			// �����ó����������µ���ɫ������
			for (uint j = 0; j < cbDesc.Variables; j++) {
				// ��ø���ɫ����������ϸ��Ϣ
				ID3D11ShaderReflectionVariable* sVariable = cb->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC vDesc;
				sVariable->GetDesc(&vDesc);
				
				std::string svName = vDesc.Name;

				// ���ø���ɫ�������ľ���λ����Ϣ
				varibleMap[svName] = ShaderVaribleLocation(bindPoint, j);

				// ���ø���ɫ�������ڳ����������е��±�����Ӧ����ϸ��Ϣ
				varibleArray[bindPoint][j] = vDesc;
			}

			#pragma endregion

		} else if(bindDesc.Type == D3D_SIT_STRUCTURED){
			
			#pragma region ���Զ������ݽṹ���з���
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
	// ��һ��,�Ѹ��е���ɫ�����󶨵���Ⱦ������
	if (vertexShader) context->VSSetShader(vertexShader.Get(),0,0);
	if (pixelShader) context->PSSetShader(pixelShader.Get(),0,0);
	if (geometryShader) context->GSSetShader(geometryShader.Get(),0,0);

	// �ڶ���,���ó���������
	for (int i = 0; i < 15; i++) {
		// ����ò�λ�����ݱ�CPU���¹�,
		// ��ô����Ҫ��������ͬ�����µ�GPU��
		if (isDirty[i]) {
			// ��ԭ
			isDirty[i] = false;

			// ��ȡ�ò�λ��Ӧ�ĳ���������
			std::string cbName = slot2CbNameMap[i];
			void* bufferData = constantBufferDatas[i];
			ComPtr<ID3D11Buffer> buffer = constantBuffers[i];

			D3D11_MAPPED_SUBRESOURCE mappedData;
			// ������ӳ�䵽GPU��
			HR(context->Map(buffer.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&mappedData));
			memcpy(mappedData.pData,bufferData, cbDescMap[i].Size);
			context->Unmap(buffer.Get(),0);

			// ���ݸò�λ������������Ӧ����ɫ��,����Ӧ����ɫ���������ó���������
			if ((cb2shaderType[i] | VertexShader) == cb2shaderType[i]) 
				context->VSSetConstantBuffers(i,1,buffer.GetAddressOf());
			
			if ((cb2shaderType[i] | PixelShader) == cb2shaderType[i])
				context->PSSetConstantBuffers(i,1,buffer.GetAddressOf());
			
			if ((cb2shaderType[i] | GeometryShader) == cb2shaderType[i])
				context->GSSetConstantBuffers(i,1,buffer.GetAddressOf());
		}
	}
	
	// ����������������Զ���Ļ��״̬���դ��״̬����Ȳ���״̬����������
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

	// ���Ĳ������ö������벼��	
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