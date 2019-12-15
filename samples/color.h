#pragma once

struct LinearColor
{
	float r, g, b, a;

	LinearColor(){}
	LinearColor(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}
};