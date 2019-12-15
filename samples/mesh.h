#pragma once

#include <string>
#include <map>
#include <vector>

#include "material.h"
#include "vector.h"

class Mesh
{
public:
	Mesh() {}
	~Mesh() {}

	struct Vertex
	{
		Vector		Position;
		Vector		Normal;
		Vector2		Tex;
	};

	struct VertexIndex
	{
		int PositonIndex;
		int NormalIndex;
		int TexCoordIndex;
	};

	struct Face
	{
		VertexIndex v1;
		VertexIndex v2;
		VertexIndex v3;
		VertexIndex v4;

		int VerticesCount;
	};

	struct SubMesh
	{
		std::string MaterialName;
		std::vector<Vertex> Vertices;
		std::vector<unsigned short> Indices;
	};

	bool LoadFromObj(const char* filename);

	bool ParseMaterial(const char* filename);

	bool ParseLine(const class std::string& line);
	bool ParseName(const std::string& line, std::string& Name);
	bool ParsePosition(const std::string& line, Vector& Position);
	bool ParseNormal(const std::string& line, Vector& Normal);
	bool ParseTextCoord2D(const std::string& line, Vector2& TexCoord);
	bool ParseColor(const std::string& line, LinearColor& Color);
	bool ParseScalar(const std::string& line, float& Value);
	bool ParseFace(const std::string& line, Face& F);
	bool ParseVertexIndex(const std::string& line, VertexIndex& Index);
	void AssembleSubMesh(SubMesh* CurrentSubMesh,const Face& F);


	const std::map<std::string, SubMesh>& GetSubMeshes() const
	{
		return SubMeshes;
	}
	const Material& GetMaterial(const std::string& name)
	{
		return Materials[name];
	}
	const Texture2D* GetTexture2D(const std::string& name)
	{
		return Textures[name];
	}
private:
	std::vector<Vector> Positions;
	std::vector<Vector> Normals;
	std::vector<Vector2> TexCoords;

	std::map<std::string, SubMesh> SubMeshes;
	std::map<std::string, Material> Materials;
	std::map<std::string, Texture2D*> Textures;
	SubMesh* CurrentSubMesh;
	Material* CurrentMaterial;
	std::string ResourceDir;
};