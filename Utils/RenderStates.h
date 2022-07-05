#pragma once

#include "D3DUtils.h"

/*
	�������������Ⱦ״̬����դ��״̬������ģʽ�����ģ��״̬�ȣ�
	����״̬��Init�����󴴽��������ڳ����и���
*/
class RenderStates {
public:
	static ComPtr<ID3D11SamplerState> defaultSamplerState;
	static ComPtr<ID3D11SamplerState> shadowMapSamplerState;
	static ComPtr<ID3D11SamplerState> repeatSamplerState;
	static ComPtr<ID3D11SamplerState> fogSamplerState;

	// �޳�����Ĺ�դ��״̬,һ��������ȾShadowMap
	static ComPtr<ID3D11RasterizerState> CullFrontRasterState;

	void Init(ComPtr<ID3D11Device> device);
};