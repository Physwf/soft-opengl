#include "Mesh.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <cmath>

std::string get_filename(const std::string& fullfilename, std::string& DirName)
{
	std::string::size_type pos1 = fullfilename.rfind('/');
	if (pos1 != std::string::npos)
	{
		DirName = fullfilename.substr(0, pos1);
		return fullfilename.substr(pos1);
	}
	else
	{
		DirName = ".";
		return fullfilename;
	}
}

bool Mesh::LoadFromObj(const char* filename)
{
	get_filename(filename,ResourceDir);

	std::string line;
	std::ifstream fs(filename);
	if (fs)
	{
		while (std::getline(fs, line))
		{
			if(line.length() == 0) continue;
			if (!ParseLine(line)) return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}


bool Mesh::ParseMaterial(const char* filename)
{
	std::string line;
	std::ifstream fs(ResourceDir + "/" + filename);
	if (fs)
	{
		while (std::getline(fs, line))
		{
			if (line.length() == 0) continue;
			if (!ParseLine(line)) return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool string_startwith(const std::string& str, const char* start)
{
	auto start_len = strlen(start);
	if (str.length() < start_len) return false;
	for (std::string::size_type i = 0; i < start_len; ++i)
	{
		if(str[i] == start[i]) continue;
		return false;
	}
	return true;
}

bool string_split(const std::string& str,const std::string& dilimer, std::vector<std::string>& split_result)
{
	std::string::size_type pos1, pos2;
	pos2 = str.find(dilimer);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		split_result.push_back(str.substr(pos1, pos2 - pos1));

		pos1 = pos2 + dilimer.size();
		pos2 = str.find(dilimer, pos1);
	}
	if (pos1 != str.length())
		split_result.push_back(str.substr(pos1));
	return split_result.size() > 0;
}

bool Mesh::ParseLine(const class std::string& line)
{
	if (line.length() <= 0) return false;
	if (string_startwith(line, "#"))
	{
	}
	else if (string_startwith(line, "mtllib "))
	{
		std::string filename;
		if (!ParseName(line, filename)) return false;
		if (!ParseMaterial(filename.c_str())) return false;
	}
	else if (string_startwith(line,"g "))
	{
		std::string GroupName;
		if(!ParseName(line, GroupName)) return false;
		if (GroupName != "default")
		{
			SubMeshes[GroupName] = SubMesh();
			CurrentSubMesh = &SubMeshes[GroupName];
		}
	}
	else if (string_startwith(line, "usemtl"))
	{
		std::string MaterialName;
		if (!ParseName(line, MaterialName)) return false;
		CurrentSubMesh->MaterialName = MaterialName;
	}
	else if (string_startwith(line, "v "))
	{
		Vector Position;
		if (!ParsePosition(line, Position)) return false;
		Positions.push_back(Position);
	}
	else if (string_startwith(line, "vn "))
	{
		Vector Normal;
		if (!ParseNormal(line, Normal)) return false;
		Normals.push_back(Normal);
	}
	else if (string_startwith(line, "vt "))
	{
		Vector2 TexCoord;
		if (!ParseTextCoord2D(line, TexCoord)) return false;
		TexCoords.push_back(TexCoord);
	}
	else if (string_startwith(line, "f "))
	{
		Face F;
		if (!ParseFace(line, F)) return false;
		AssembleSubMesh(CurrentSubMesh, F);
	}
	else if (string_startwith(line, "newmtl "))
	{
		std::string MaterialName;
		if (!ParseName(line, MaterialName)) return false;
		Materials[MaterialName] = Material();
		CurrentMaterial = &Materials[MaterialName];
	}
	else if (string_startwith(line, "Kd "))
	{
		LinearColor Color;
		if (!ParseColor(line, Color)) return false;
		CurrentMaterial->Kd = Color;
	}
	else if (string_startwith(line, "Ka "))
	{
		LinearColor Color;
		if (!ParseColor(line, Color)) return false;
		CurrentMaterial->Ka = Color;
	}
	else if (string_startwith(line, "Tf "))
	{
		LinearColor Color;
		if (!ParseColor(line, Color)) return false;
		CurrentMaterial->Tf = Color;
	}
	else if (string_startwith(line, "Ni "))
	{
		float Value;
		if (!ParseScalar(line, Value)) return false;
		CurrentMaterial->Ni = Value;
	}
	else if (string_startwith(line, "Ks "))
	{
		LinearColor Color;
		if (!ParseColor(line, Color)) return false;
		CurrentMaterial->Ks = Color;
	}
	else if (string_startwith(line, "Ns "))
	{
		float Value;
		if (!ParseScalar(line, Value)) return false;
		CurrentMaterial->Ns = Value;
	}
	else if (string_startwith(line, "map_Kd "))
	{
		std::string FileName;
		if (!ParseName(line, FileName)) return false;
		CurrentMaterial->map_Kd = ResourceDir + "/" + FileName;
		Texture2D* tex = new Texture2D();
		tex->LoadFromJpg(CurrentMaterial->map_Kd.c_str());
		tex->UpdateRHI();
		Textures[CurrentMaterial->map_Kd] = tex;
	}
	else if (string_startwith(line, "map_Ks "))
	{
		std::string FileName;
		if (!ParseName(line, FileName)) return false;
		CurrentMaterial->map_Ks = ResourceDir + "/" + FileName;
		Texture2D* tex = new Texture2D();
		tex->LoadFromJpg(CurrentMaterial->map_Ks.c_str());
		tex->UpdateRHI();
		Textures[CurrentMaterial->map_Ks] = tex;
	}
	return true;
}

bool Mesh::ParseName(const std::string& line,std::string& Name)
{
	std::string::size_type pos1 = line.find(' ');
	if (pos1 != std::string::npos)
	{
		Name = line.substr(pos1+1);
		return Name.size() > 0;
	}
	return false;
}

bool Mesh::ParsePosition(const std::string& line, Vector& Position)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 3)
		{
			Position.x = (float)std::atof(result[1].c_str());
			Position.y = (float)std::atof(result[2].c_str());
			Position.z = (float)std::atof(result[3].c_str());
			return true;
		}
	}
	return false;
}


bool Mesh::ParseNormal(const std::string& line, Vector& Normal)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 3)
		{
			Normal.x = (float)std::atof(result[1].c_str());
			Normal.y = (float)std::atof(result[2].c_str());
			Normal.z = (float)std::atof(result[3].c_str());
			return true;
		}
	}
	return false;
}

bool Mesh::ParseTextCoord2D(const std::string& line,Vector2& TexCoord)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 2)
		{
			TexCoord.x = (float)std::atof(result[1].c_str());
			TexCoord.y = (float)std::atof(result[2].c_str());
			return true;
		}
	}
	return false;
}

bool Mesh::ParseColor(const std::string& line, LinearColor& Color)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 3)
		{
			Color.r = (float)std::atof(result[1].c_str());
			Color.g = (float)std::atof(result[2].c_str());
			Color.b = (float)std::atof(result[3].c_str());
			return true;
		}
	}
	return false;
}


bool Mesh::ParseScalar(const std::string& line, float& Value)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 1)
		{
			Value = (float)std::atof(result[1].c_str());
			return true;
		}
	}
	return false;
}

bool Mesh::ParseFace(const std::string& line, Face& F)
{
	std::vector<std::string> result;
	if (string_split(line, " ", result))
	{
		if (result.size() > 3)
		{
			VertexIndex Index;
			F.VerticesCount = 0;
			if (!ParseVertexIndex(result[1], Index)) return false;
			F.v1 = Index;
			++F.VerticesCount;
			if (!ParseVertexIndex(result[2], Index)) return false;
			F.v2 = Index;
			++F.VerticesCount;
			if (!ParseVertexIndex(result[3], Index)) return false;
			F.v3 = Index;
			++F.VerticesCount;
			if (result.size() > 4)
			{
				if (!ParseVertexIndex(result[4], Index)) return false;
				F.v4 = Index;
				++F.VerticesCount;
			}
			return true;
		}
	}
	return false;
}

bool Mesh::ParseVertexIndex(const std::string& line, VertexIndex& Index)
{
	std::vector<std::string> result;
	if (string_split(line, "/", result))
	{
		if (result.size() >= 3)
		{
			Index.PositonIndex = std::atoi(result[0].c_str());
			Index.TexCoordIndex = std::atoi(result[1].c_str());
			Index.NormalIndex = std::atoi(result[2].c_str());
			return true;
		}
		else if (result.size() >= 2)
		{
			Index.PositonIndex = std::atoi(result[0].c_str());
			Index.TexCoordIndex = std::atoi(result[1].c_str());
			return true;
		}
	}
	return false;
}

void Mesh::AssembleSubMesh(SubMesh* pSubMesh, const Face& F)
{
	if (pSubMesh)
	{
		Vertex V1, V2, V3, V4;
		V1.Position = Positions[F.v1.PositonIndex-1];
		V2.Position = Positions[F.v2.PositonIndex - 1];
		V3.Position = Positions[F.v3.PositonIndex - 1];
		if (F.VerticesCount == 4)
		{
			V4.Position = Positions[F.v4.PositonIndex - 1];
		}

		if (TexCoords.size() > 0)
		{
			V1.Tex = TexCoords[F.v1.TexCoordIndex - 1];
			V2.Tex = TexCoords[F.v2.TexCoordIndex - 1];
			V3.Tex = TexCoords[F.v3.TexCoordIndex - 1];
			if (F.VerticesCount == 4)
			{
				V4.Tex = TexCoords[F.v4.TexCoordIndex - 1];
			}
		}
		if (Normals.size() > 0)
		{
			V1.Normal = Normals[F.v1.NormalIndex - 1];
			V2.Normal = Normals[F.v2.NormalIndex - 1];
			V3.Normal = Normals[F.v3.NormalIndex - 1];
			if (F.VerticesCount == 4)
			{
				V4.Normal = Normals[F.v4.NormalIndex - 1];
			}
		}
		else
		{
			Vector Normal = multiply(sub(V3.Position, V1.Position), sub(V2.Position, V1.Position));

			Normal = normalize(Normal);
			V1.Normal = Normal;
			V2.Normal = Normal;
			V3.Normal = Normal;
			if (F.VerticesCount == 4)
			{
				V4.Normal = Normal;
			}
		}

		pSubMesh->Vertices.push_back(V1);
		pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
		pSubMesh->Vertices.push_back(V2);
		pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
		pSubMesh->Vertices.push_back(V3);
		pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);

		if (F.VerticesCount == 4)
		{
			pSubMesh->Vertices.push_back(V1);
			pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
			pSubMesh->Vertices.push_back(V3);
			pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
			pSubMesh->Vertices.push_back(V4);
			pSubMesh->Indices.push_back((unsigned short)pSubMesh->Vertices.size() - 1);
		}
	}
}

