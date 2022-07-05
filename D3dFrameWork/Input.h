#pragma once

#include "D3DUtils.h"

// ���ڹ�����Ϸ����������(���/���̵�)
class Input {
public:
	// ��������ƫ��
	static float2 mousePosOffset;
	// ��һ֡�������
	static float2 lastFrameMousePos;
	// Ϊ������ʾ��ǰת,Ϊ������ʾ���ת
	static int mouseWheelState;
public:
	// �ж��Ƿ���ĳ������
	static bool GetKeyDown(int vKey);

	// ���ص�ǰ����������һ֡�ƶ���ƫ����
	// ����ֵ��float2����,��һ��ֵΪx����ƫ����,�ڶ���ֵΪy����ƫ����
	static float2 GetMouseMove();
	
};