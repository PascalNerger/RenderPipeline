#pragma once

#include <glm/glm.hpp>

#include "PBR/Core/RefCounting.h"
#include "PBR/RHI/VertexArray.h"


namespace EngineCore
{


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Vertex //////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct Vertex
	{

		glm::fvec3 Position;
		glm::fvec2 UvCoord;
		glm::fvec3 Normal;
		glm::fvec3 Binormal;
		glm::fvec3 Tangent;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// MeshData ////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class MeshData
	{

	public:

		MeshData() = default;
		~MeshData() = default;

		inline void AddVertex(const Vertex& vertex) { Vertices.push_back(vertex); }
		inline void AddIndex(const uint32 index) { Indices.push_back(index); }

		inline const std::vector<Vertex>& GetVertices() const { return Vertices; }
		inline const std::vector<uint32>& GetIndices() const { return Indices; }

	private:

		std::vector<Vertex> Vertices;
		std::vector<uint32> Indices;

	};


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Mesh ////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Mesh : public RefCountedObject
	{

	public:

		Mesh();
		Mesh(const VertexArrayRef& vertexArray);
		~Mesh();


		inline void SetVertexArray(const VertexArrayRef& vertexArray) { VertexArrayObject = vertexArray; IndexBufferObject = vertexArray->GetIndexBuffer(); }

		inline const VertexArrayRef& GetVertexArray() const { return VertexArrayObject; }
		inline const IndexBufferRef& GetIndexBuffer() const { return IndexBufferObject; }

	private:

		void Renderer_Bind() const;
		void Renderer_Unbind() const;

	public:

		static Ref<Mesh> Create(const MeshData& data);

	private:

		VertexArrayRef VertexArrayObject;
		IndexBufferRef IndexBufferObject;

	private:

		friend class Renderer;

	};

	using MeshRef = Ref<Mesh>;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// InstanceData ////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct InstanceData
	{

		glm::fmat4 ModelMatrix;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// InstanceBuffer //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class InstanceBuffer
	{

	public:

		InstanceBuffer(const uint32 instanceCount);
		~InstanceBuffer();

		void SetInstanceData(const uint32 instance, const InstanceData& data);

		inline const VertexBufferRef& GetVertexBuffer() const { return VertexBuffer; }
		inline uint32 GetInstanceCount() const { return InstanceCount; }

	private:

		uint32 InstanceCount;
		VertexBufferRef VertexBuffer;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// InstancedMesh ///////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class InstancedMesh : public RefCountedObject
	{

	public:

		InstancedMesh();
		InstancedMesh(const VertexArrayRef& vertexArray, const uint32 maxInstances);
		~InstancedMesh();


		inline const VertexArrayRef& GetVertexArray() const { return VertexArray; }
		inline InstanceBuffer& GetInstanceBuffer() { return InstanceBufferObject; }

	private:

		void Renderer_Bind() const;
		void Renderer_Unbind() const;

	public:

		static Ref<InstancedMesh> Create(const MeshData& data, const uint32 instances);

	private:

		VertexArrayRef VertexArray;
		IndexBufferRef IndexBuffer;

		InstanceBuffer InstanceBufferObject;

	private:

		friend class Renderer;

	};

	using InstancedMeshRef = Ref<InstancedMesh>;

}