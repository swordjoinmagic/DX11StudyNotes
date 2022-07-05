#pragma once

#include "D3DUtils.h"

// 用于管理游戏中所有输入(鼠标/键盘等)
class Input {
public:
	// 鼠标坐标的偏移
	static float2 mousePosOffset;
	// 上一帧鼠标坐标
	static float2 lastFrameMousePos;
	// 为正数表示向前转,为负数表示向后转
	static int mouseWheelState;
public:
	// 判断是否按下某个按键
	static bool GetKeyDown(int vKey);

	// 返回当前鼠标相比于上一帧移动的偏移量
	// 返回值是float2类型,第一个值为x坐标偏移量,第二个值为y坐标偏移量
	static float2 GetMouseMove();
	
};