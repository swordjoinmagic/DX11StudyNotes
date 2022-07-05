#pragma once

/*
	用于保存常量缓冲区中常用变量名称的类
*/
class CBVaribleConfig {
public:
	// mvp矩阵的名称
	const static char* mvpMatrix;
	// model矩阵的名称
	const static char* modelMatrix;
	// model的逆转置矩阵的名称
	const static char* transInvModelMarix;
	// view矩阵的名称
	const static char* viewMatrix;
	// proj矩阵的名称
	const static char* projMatrix;
	// 观察者坐标的名称
	const static char* viewPos;
	// 漫反射颜色的名称
	const static char* mainColor;
	// 平行光光源的名称
	const static char* light;
	// 阴影贴图的名称
	const static char* shadowMap;
	// 阴影贴图采样状态的名称
	const static char* shadowSamplerState;
	// 光源变换矩阵的名称
	const static char* lightMatrix;
	// 主要贴图的名称
	const static char* mainTexture;
	// 材质颜色
	const static char* albedoLocation;
	// 金属度
	const static char* metallicLocation;
	// 环境光遮蔽
	const static char* aoLocation;
	// 粗糙度
	const static char* roughness;
	// 辐照度贴图
	const static char* irradianceMap;
};