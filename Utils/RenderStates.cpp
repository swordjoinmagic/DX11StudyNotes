#include "RenderStates.h"

ComPtr<ID3D11SamplerState> RenderStates::defaultSamplerState = nullptr;
ComPtr<ID3D11SamplerState> RenderStates::shadowMapSamplerState = nullptr;
ComPtr<ID3D11RasterizerState> RenderStates::CullFrontRasterState = nullptr;
ComPtr<ID3D11SamplerState> RenderStates::repeatSamplerState = nullptr;
ComPtr<ID3D11SamplerState> RenderStates::fogSamplerState = nullptr;

void RenderStates::Init(ComPtr<ID3D11Device> device) {
	// 初始化默认采样状态（即Clamp）
	CD3D11_SAMPLER_DESC defaultSamplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	HR(device->CreateSamplerState(&defaultSamplerDesc,defaultSamplerState.GetAddressOf()));
	
	// 初始化阴影采样状态
	CD3D11_SAMPLER_DESC shadowMapSamplerStateDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	shadowMapSamplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	shadowMapSamplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerStateDesc.BorderColor[0] = 1;
	shadowMapSamplerStateDesc.BorderColor[1] = 1;
	shadowMapSamplerStateDesc.BorderColor[2] = 1;
	shadowMapSamplerStateDesc.BorderColor[3] = 1;
	HR(device->CreateSamplerState(&shadowMapSamplerStateDesc,shadowMapSamplerState.GetAddressOf()));

	// 初始化重复采样状态
	CD3D11_SAMPLER_DESC wrapDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	wrapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	wrapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	wrapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	HR(device->CreateSamplerState(&wrapDesc,repeatSamplerState.GetAddressOf()));

	// 初始化雾采样的状态
	CD3D11_SAMPLER_DESC fogSamplerStateDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	fogSamplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	fogSamplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerStateDesc.BorderColor[0] = 100;
	shadowMapSamplerStateDesc.BorderColor[1] = 100;
	shadowMapSamplerStateDesc.BorderColor[2] = 100;
	shadowMapSamplerStateDesc.BorderColor[3] = 100;

	// ===========================
	// 初始化正面剔除光栅化状态
	CD3D11_RASTERIZER_DESC cullFrontDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	cullFrontDesc.CullMode = D3D11_CULL_FRONT;	// 剔除正面
	HR(device->CreateRasterizerState(&cullFrontDesc,CullFrontRasterState.GetAddressOf()));
}