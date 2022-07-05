#pragma once

/*
	���ڱ��泣���������г��ñ������Ƶ���
*/
class CBVaribleConfig {
public:
	// mvp���������
	const static char* mvpMatrix;
	// model���������
	const static char* modelMatrix;
	// model����ת�þ��������
	const static char* transInvModelMarix;
	// view���������
	const static char* viewMatrix;
	// proj���������
	const static char* projMatrix;
	// �۲������������
	const static char* viewPos;
	// ��������ɫ������
	const static char* mainColor;
	// ƽ�й��Դ������
	const static char* light;
	// ��Ӱ��ͼ������
	const static char* shadowMap;
	// ��Ӱ��ͼ����״̬������
	const static char* shadowSamplerState;
	// ��Դ�任���������
	const static char* lightMatrix;
	// ��Ҫ��ͼ������
	const static char* mainTexture;
	// ������ɫ
	const static char* albedoLocation;
	// ������
	const static char* metallicLocation;
	// �������ڱ�
	const static char* aoLocation;
	// �ֲڶ�
	const static char* roughness;
	// ���ն���ͼ
	const static char* irradianceMap;
};