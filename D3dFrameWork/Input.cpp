#include "Input.h"

float2 Input::mousePosOffset = float2(0,0);
float2 Input::lastFrameMousePos = float2(0, 0);
int Input::mouseWheelState = 0;

bool Input::GetKeyDown(int vKey) {
	return GetAsyncKeyState(vKey) < 0;
}

float2 Input::GetMouseMove() {
	return mousePosOffset;
}