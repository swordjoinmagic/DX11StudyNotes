#pragma once

#include "D3DUtils.h"

/*
	用来管理各个渲染状态（光栅化状态、采样模式、深度模板状态等）
	所有状态在Init方法后创建，用于在程序中复用
*/
class RenderStates {
public:
	static ComPtr<ID3D11SamplerState> defaultSamplerState;
	static ComPtr<ID3D11SamplerState> shadowMapSamplerState;
	static ComPtr<ID3D11SamplerState> repeatSamplerState;
	static ComPtr<ID3D11SamplerState> fogSamplerState;

	// 剔除正面的光栅化状态,一般用于渲染ShadowMap
	static ComPtr<ID3D11RasterizerState> CullFrontRasterState;

	void Init(ComPtr<ID3D11Device> device);
};