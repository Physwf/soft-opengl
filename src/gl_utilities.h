/* Copyright(C) 2019-2020 Physwf

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < http://www.gnu.org/licenses/>.
*/

#pragma once

template <typename T>
T glClamp(T value, T min, T max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
};

template <typename T> 
T glAbs(T value)
{
	if (value < 0) return -value;
	else return value;
}

template <typename T>
T glMin(T v1, T v2)
{
	return v1 <= v2 ? v1 : v2;
};

template <typename T, typename ... Args>
T glMin(T v1, T v2, Args... args)
{
	return glMin(glMin(v1, v2), args...);
};

template <typename T>
T glMax(T v1, T v2)
{
	return v1 <= v2 ? v2 : v1;
};

template <typename T, typename ... Args>
T glMax(T v1, T v2, Args... args)
{
	return glMax(glMax(v1, v2), args...);
};
