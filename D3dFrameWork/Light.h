#pragma once

#include "D3DUtils.h"

// ƽ�й�
struct Light {
public:
	// ��������˵��ƽ�йⲻ�߱�λ������
	// ����Ϊ����Ⱦ��Ӱ��ͼ����ù�Դ��VP������ʱ����һ��
	float3 pos;
	// ƽ�йⷽ��,
	float3 dir;	
	float3 lightColor;

	Light() { ZeroMemory(this, sizeof(this)); }
};

// ���Դ
struct PointLight {
public:
	float3 pos;
	float3 lightColor;
	float Kc;	// ˥��������
	float KI;	// ˥��һ����
	float Kq;	// ˥��������

	PointLight() { ZeroMemory(this, sizeof(this)); }
};

// �۹��
struct SpotLight {
public:
	float3 pos;
	// ��ʾ�۹�Ƴ�����spotDir
	float3 dir;
	float3 lightColor;
	float Kc;	// ˥��������
	float KI;	// ˥��һ����
	float Kq;	// ˥��������
	// ��,�۹�����
	float Phi;
	// ��,�۹���ڽ�,
	float Theta;

	SpotLight() { ZeroMemory(this, sizeof(this)); }
};