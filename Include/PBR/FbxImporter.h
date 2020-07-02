#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "fbxsdk.h"
#include "PBR/Core/BaseTypes.h"

#include "PBR/Engine/Mesh.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// FbxHelper ///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class FbxHelper
	{

	public:

		FbxHelper();
		~FbxHelper();

		MeshData Import(const std::string& file);

	private:

		MeshData CreateMeshData() const;

		void GetVertices(FbxMesh* mesh);
		void GetUvCoords(FbxMesh* mesh);
		void GetNormals(FbxMesh* mesh);
		void GetBinormals(FbxMesh* mesh);
		void GetTangents(FbxMesh* mesh);
		void GetIndices(FbxMesh* mesh);

		void ClearBuffers();

	private:

		FbxManager* Manager;
		FbxIOSettings* Settings;
		FbxImporter* Importer;

		std::vector<glm::vec3> PositionBuffer;
		std::vector<glm::vec2> UvCoordBuffer;
		std::vector<glm::vec3> NormalBuffer;
		std::vector<glm::vec3> BinormalBuffer;
		std::vector<glm::vec3> TangentBuffer;
		std::vector<int32> IndexBuffer;

	};

}