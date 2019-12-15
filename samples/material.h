#pragma once

#include <string>
#include <memory>

#include "texture.h"
#include "color.h"
/*
Ft	Fresnel reflectance
Ft	Fresnel transmittance
Ia	ambient light
I	light intensity
Ir	intensity from reflected direction
(reflection map and/or ray tracing)
It	intensity from transmitted direction
Ka	ambient reflectance
Kd	diffuse reflectance
Ks	specular reflectance
Tf	transmission filter
*/
struct Material
{
	std::string Name;

	LinearColor Ka;//ambient
	std::string map_Ka;
	LinearColor Kd;//diffuse
	std::string map_Kd;
	LinearColor Ks;//specular
	std::string map_Ks;
	LinearColor Tf;//transmission filter
	/*
	Specifies the specular exponent for the current material.  This defines
	the focus of the specular highlight.

	 "exponent" is the value for the specular exponent.  A high exponent
	results in a tight, concentrated highlight.  Ns values normally range
	from 0 to 1000.
	*/
	float Ns;
	/*
	Specifies the optical density for the surface.  This is also known as
	index of refraction.

	 "optical_density" is the value for the optical density.  The values can
	range from 0.001 to 10.  A value of 1.0 means that light does not bend
	as it passes through an object.  Increasing the optical_density
	increases the amount of bending.  Glass has an index of refraction of
	about 1.5.  Values of less than 1.0 produce bizarre results and are not
	recommended.
	*/
	float Ni;
};