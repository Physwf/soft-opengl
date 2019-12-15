#pragma once

#include "light.h"

#include <vector>

class Scene
{
public:
	Scene() {}
	~Scene() {}

	void Init();

	void Draw();
private:
	AmbientLight		AL;
	DirctionalLight		DL;
	std::vector<PointLight> PLs;
};