#pragma once

#include "D3DUtils.h"

// 平行光
struct Light {
public:
	// 理论上来说，平行光不具备位置属性
	// 这里为了渲染阴影贴图（获得光源的VP矩阵）临时设置一个
	float3 pos;
	// 平行光方向,
	float3 dir;	
	float3 lightColor;

	Light() { ZeroMemory(this, sizeof(this)); }
};

// 点光源
struct PointLight {
public:
	float3 pos;
	float3 lightColor;
	float Kc;	// 衰减常数项
	float KI;	// 衰减一次项
	float Kq;	// 衰减二次项

	PointLight() { ZeroMemory(this, sizeof(this)); }
};

// 聚光灯
struct SpotLight {
public:
	float3 pos;
	// 表示聚光灯朝向方向spotDir
	float3 dir;
	float3 lightColor;
	float Kc;	// 衰减常数项
	float KI;	// 衰减一次项
	float Kq;	// 衰减二次项
	// φ,聚光灯外角
	float Phi;
	// θ,聚光灯内角,
	float Theta;

	SpotLight() { ZeroMemory(this, sizeof(this)); }
};