#pragma once

#include "color.h"
#include "vector.h"

struct Light
{
	float fBrightness;
	LinearColor Color;
};

struct AmbientLight : public Light
{

};

struct DirctionalLight : public Light
{
	Vector Direction;
};

struct PointLight : public Light
{
	Vector Position;
	float Range;
};