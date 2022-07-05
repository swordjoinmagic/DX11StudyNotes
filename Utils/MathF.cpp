#include "D3DUtils.h"
#include "MathF.h"

const float MathF::PI = XM_PI;
const float MathF::Deg2Rad = MathF::PI / 180.0f;

float MathF::Radians(float angle) {
	return angle * MathF::Deg2Rad;
}

float MathF::Clamp(float value,float min,float max) {
	if (value > max) return max;
	if (value < min) return min;
	return value;
}