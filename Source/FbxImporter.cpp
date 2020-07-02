#include "pch.h"

#include "PBR/FbxImporter.h"


namespace EngineCore
{

	FbxHelper::FbxHelper()
		: Manager(nullptr), Settings(nullptr), Importer(nullptr)
	{
		Manager = FbxManager::Create();
		Settings = FbxIOSettings::Create(Manager, IOSROOT);

		Importer = FbxImporter::Create(Manager, "");
	}

	FbxHelper::~FbxHelper()
	{
		Manager->Destroy();
	}

	MeshData FbxHelper::Import(const std::string& file)
	{
		ClearBuffers();

		if (!Importer->Initialize(file.c_str(), -1, Manager->GetIOSettings()))
		{
			std::cout << Importer->GetStatus().GetErrorString() << std::endl;
			return MeshData();
		}

		FbxScene* scene = FbxScene::Create(Manager, "Scene");
		Importer->Import(scene);

		FbxGeometryConverter fbxConverter(Manager);
		const bool triangulateResult = fbxConverter.Triangulate(scene, true);

		if (!triangulateResult)
			std::cout << "Failed to triangulate scene!" << std::endl;

		FbxNode* rootNode = scene->GetRootNode();
		const int32 childCount = rootNode->GetChildCount();


		for (int32 i = 0; i < childCount; ++i)
		{
			FbxNode* child = rootNode->GetChild(i);
			FbxMesh* mesh = child->GetMesh();

			GetVertices(mesh);
			GetUvCoords(mesh);
			GetNormals(mesh);
			GetBinormals(mesh);
			GetTangents(mesh);
			GetIndices(mesh);
		}

		scene->Destroy();
		return CreateMeshData();
	}

	MeshData FbxHelper::CreateMeshData() const
	{
		MeshData data;

		// TODO: This way the indices are resolved, so there is no memory saving through indices, i.e. they are useless.
		// This should not be the case!
		for (uint32 i = 0; i < IndexBuffer.size(); ++i)
		{
			const uint32 index = IndexBuffer[i];

			Vertex vertex;
			vertex.Position = PositionBuffer[index];
			vertex.UvCoord = UvCoordBuffer[i];
			vertex.Normal = NormalBuffer[i];
			vertex.Binormal = BinormalBuffer[i];
			vertex.Tangent = TangentBuffer[i];

			data.AddVertex(vertex);
			data.AddIndex(i);
		}

		return data;
	}

	void FbxHelper::GetVertices(FbxMesh* mesh)
	{
		const int32 vertexCount = mesh->GetControlPointsCount();
		for (int32 i = 0; i < vertexCount; ++i)
		{
			const FbxVector4 vertex = mesh->GetControlPointAt(i);

			const FbxDouble x = vertex.mData[0];
			const FbxDouble y = vertex.mData[1];
			const FbxDouble z = vertex.mData[2];
			const FbxDouble w = vertex.mData[3];

			PositionBuffer.emplace_back(x, y, z);
		}
	}

	void FbxHelper::GetUvCoords(FbxMesh* mesh)
	{
		FbxStringList uvSetNameList;
		mesh->GetUVSetNames(uvSetNameList);

		for (int32 uvSetIndex = 0; uvSetIndex < uvSetNameList.GetCount(); ++uvSetIndex)
		{
			const char* uvSetName = uvSetNameList.GetStringAt(uvSetIndex);
			const FbxGeometryElementUV* uvElement = mesh->GetElementUV(uvSetName);

			if (!uvElement)
				continue;

			if (uvElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex && uvElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
				return;

			const bool useIndex = uvElement->GetReferenceMode() != FbxGeometryElement::eDirect;
			const int32 indexCount = useIndex ? uvElement->GetIndexArray().GetCount() : 0;

			const int32 polygonCount = mesh->GetPolygonCount();
			if (uvElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				for (int32 polygonIndex = 0; polygonIndex < polygonIndex; ++polygonIndex)
				{
					const int32 polygonSize = mesh->GetPolygonSize(polygonIndex);
					for (int32 vertexIndex = 0; vertexIndex < polygonSize; ++vertexIndex)
					{
						const int32 polygonVertexIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
						const int32 uvIndex = useIndex ? uvElement->GetIndexArray().GetAt(polygonVertexIndex) : polygonVertexIndex;

						const FbxVector2 uvCoord = uvElement->GetDirectArray().GetAt(uvIndex);
						const FbxDouble u = uvCoord.mData[0];
						const FbxDouble v = uvCoord.mData[1];

						UvCoordBuffer.emplace_back(u, v);
					}
				}
			}
			else if (uvElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				int32 polygonIndexCounter = 0;
				for (int32 polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
				{
					const int32 polygonSize = mesh->GetPolygonSize(polygonIndex);
					for (int32 vertexIndex = 0; vertexIndex < polygonSize; ++vertexIndex)
					{
						if (polygonIndexCounter < indexCount)
						{
							const int32 uvIndex = useIndex ? uvElement->GetIndexArray().GetAt(polygonIndexCounter) : polygonIndexCounter;

							const FbxVector2 uvCoord = uvElement->GetDirectArray().GetAt(uvIndex);
							const FbxDouble u = uvCoord.mData[0];
							const FbxDouble v = uvCoord.mData[1];

							UvCoordBuffer.emplace_back(u, v);

							++polygonIndexCounter;
						}
					}
				}
			}
		}
	}

	void FbxHelper::GetNormals(FbxMesh* mesh)
	{
		const FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();

		if (!normalElement)
			return;

		if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int32 vertexIndex = 0; vertexIndex < mesh->GetControlPointsCount(); ++vertexIndex)
			{
				int32 normalIndex = 0;
				if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
					normalIndex = vertexIndex;

				if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					normalIndex = normalElement->GetIndexArray().GetAt(vertexIndex);

				const FbxVector4 normal = normalElement->GetDirectArray().GetAt(normalIndex);
				const FbxDouble x = normal.mData[0];
				const FbxDouble y = normal.mData[1];
				const FbxDouble z = normal.mData[2];
				const FbxDouble w = normal.mData[3];

				NormalBuffer.emplace_back(x, y, z);
			}
		}
		else if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int32 indexByPolygonVertex = 0;

			for (int32 polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); ++polygonIndex)
			{
				const int32 polygonSize = mesh->GetPolygonSize(polygonIndex);

				for (int32 i = 0; i < polygonSize; ++i)
				{
					int32 normalIndex = 0;

					if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						normalIndex = indexByPolygonVertex;

					if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						normalIndex = normalElement->GetIndexArray().GetAt(indexByPolygonVertex);


					const FbxVector4 normal = normalElement->GetDirectArray().GetAt(normalIndex);
					const FbxDouble x = normal.mData[0];
					const FbxDouble y = normal.mData[1];
					const FbxDouble z = normal.mData[2];
					const FbxDouble w = normal.mData[3];

					NormalBuffer.emplace_back(x, y, z);

					++indexByPolygonVertex;
				}
			}
		}
	}

	void FbxHelper::GetBinormals(FbxMesh* mesh)
	{
		const FbxGeometryElementBinormal* binormalElement = mesh->GetElementBinormal();

		if (!binormalElement)
			return;

		if (binormalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int32 vertexIndex = 0; vertexIndex < mesh->GetControlPointsCount(); ++vertexIndex)
			{
				int32 binormalIndex = 0;
				if (binormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
					binormalIndex = vertexIndex;

				if (binormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					binormalIndex = binormalElement->GetIndexArray().GetAt(vertexIndex);

				const FbxVector4 binormal = binormalElement->GetDirectArray().GetAt(binormalIndex);
				const FbxDouble x = binormal.mData[0];
				const FbxDouble y = binormal.mData[1];
				const FbxDouble z = binormal.mData[2];
				const FbxDouble w = binormal.mData[3];

				BinormalBuffer.emplace_back(x, y, z);
			}
		}
		else if (binormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int32 indexByPolygonVertex = 0;

			for (int32 polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); ++polygonIndex)
			{
				const int32 polygonSize = mesh->GetPolygonSize(polygonIndex);

				for (int32 i = 0; i < polygonSize; ++i)
				{
					int32 binormalIndex = 0;

					if (binormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						binormalIndex = indexByPolygonVertex;

					if (binormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						binormalIndex = binormalElement->GetIndexArray().GetAt(indexByPolygonVertex);


					const FbxVector4 binormal = binormalElement->GetDirectArray().GetAt(binormalIndex);
					const FbxDouble x = binormal.mData[0];
					const FbxDouble y = binormal.mData[1];
					const FbxDouble z = binormal.mData[2];
					const FbxDouble w = binormal.mData[3];

					BinormalBuffer.emplace_back(x, y, z);

					++indexByPolygonVertex;
				}
			}
		}
	}

	void FbxHelper::GetTangents(FbxMesh* mesh)
	{
		const FbxGeometryElementTangent* tangentElement = mesh->GetElementTangent();

		if (!tangentElement)
			return;

		if (tangentElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int32 vertexIndex = 0; vertexIndex < mesh->GetControlPointsCount(); ++vertexIndex)
			{
				int32 tangentIndex = 0;
				if (tangentElement->GetReferenceMode() == FbxGeometryElement::eDirect)
					tangentIndex = vertexIndex;

				if (tangentElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					tangentIndex = tangentElement->GetIndexArray().GetAt(vertexIndex);

				const FbxVector4 tangent = tangentElement->GetDirectArray().GetAt(tangentIndex);
				const FbxDouble x = tangent.mData[0];
				const FbxDouble y = tangent.mData[1];
				const FbxDouble z = tangent.mData[2];
				const FbxDouble w = tangent.mData[3];

				TangentBuffer.emplace_back(x, y, z);
			}
		}
		else if (tangentElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int32 indexByPolygonVertex = 0;

			for (int32 polygonIndex = 0; polygonIndex < mesh->GetPolygonCount(); ++polygonIndex)
			{
				const int32 polygonSize = mesh->GetPolygonSize(polygonIndex);

				for (int32 i = 0; i < polygonSize; ++i)
				{
					int32 tangentIndex = 0;

					if (tangentElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						tangentIndex = indexByPolygonVertex;

					if (tangentElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						tangentIndex = tangentElement->GetIndexArray().GetAt(indexByPolygonVertex);


					const FbxVector4 tangent = tangentElement->GetDirectArray().GetAt(tangentIndex);
					const FbxDouble x = tangent.mData[0];
					const FbxDouble y = tangent.mData[1];
					const FbxDouble z = tangent.mData[2];
					const FbxDouble w = tangent.mData[3];

					TangentBuffer.emplace_back(x, y, z);

					++indexByPolygonVertex;
				}
			}
		}
	}

	void FbxHelper::GetIndices(FbxMesh* mesh)
	{
		const int32 indexCount = mesh->GetPolygonCount();

		for (int32 i = 0; i < indexCount; ++i)
		{
			const int32 polygonSize = mesh->GetPolygonSize(i);

			for (int32 v = 0; v < polygonSize; ++v)
			{
				const int32 index = mesh->GetPolygonVertex(i, v);
				IndexBuffer.push_back(index);
			}
		}
	}

	void FbxHelper::ClearBuffers()
	{
		PositionBuffer.clear();
		UvCoordBuffer.clear();
		NormalBuffer.clear();
		IndexBuffer.clear();
	}

}