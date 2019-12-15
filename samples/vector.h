#pragma once

struct Vector
{
	float x, y, z;
};

struct Vector2
{
	float x, y;
};

Vector multiply(const Vector& v1, const Vector& v2);
Vector sub(const Vector& v1, const Vector& v2);
Vector normalize(const Vector& v);