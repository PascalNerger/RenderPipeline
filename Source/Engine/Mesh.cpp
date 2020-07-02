#include "pch.h"
#include "PBR/Engine/Mesh.h"

#include "PBR/RHI/VertexArray.h"

namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Mesh ////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Mesh::Mesh()
		: VertexArrayObject(nullptr)
		, IndexBufferObject(nullptr)
	{
		
	}

	Mesh::Mesh(const VertexArrayRef& vertexArray)
		: VertexArrayObject(vertexArray)
		, IndexBufferObject(vertexArray->GetIndexBuffer())
	{
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Renderer_Bind() const
	{
		VertexArrayObject->Bind();
		IndexBufferObject->Bind();
	}

	void Mesh::Renderer_Unbind() const
	{
		VertexArrayObject->Unbind();
		IndexBufferObject->Unbind();
	}

	Ref<Mesh> Mesh::Create(const MeshData& data)
	{
		const std::vector<Vertex>& vertices = data.GetVertices();
		const std::vector<uint32>& indices = data.GetIndices();

		VertexArrayRef vao = VertexArray::Create();
		VertexBufferRef vbo = VertexBuffer::Create(vertices.data(), sizeof(Vertex) * static_cast<uint32>(vertices.size()));
		const IndexBufferRef ibo = IndexBuffer::Create(indices.data(), static_cast<uint32>(indices.size()));

		static const BufferLayout layout = {
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float2 },
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float3 }
		};

		vbo->SetLayout(layout);

		vao->AddVertexBuffer(vbo);
		vao->SetIndexBuffer(ibo);

		return new Mesh(vao);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// InstanceBuffer //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	InstanceBuffer::InstanceBuffer(const uint32 instanceCount)
		: InstanceCount(instanceCount)
		, VertexBuffer(VertexBuffer::CreateMapped(instanceCount * sizeof(InstanceData)))
	{
		static const BufferLayout layout = {
			{ ShaderDataType::Float4, 1 },
			{ ShaderDataType::Float4, 1 },
			{ ShaderDataType::Float4, 1 },
			{ ShaderDataType::Float4, 1 }
		};

		VertexBuffer->SetLayout(layout);
	}

	InstanceBuffer::~InstanceBuffer()
	{

	}

	void InstanceBuffer::SetInstanceData(const uint32 instance, const InstanceData& data)
	{
		VertexBuffer->SetData(&data, instance * sizeof(InstanceData), sizeof(InstanceData));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// InstancedMesh ///////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	InstancedMesh::InstancedMesh()
		: VertexArray(nullptr)
		, IndexBuffer(nullptr)
		, InstanceBufferObject(0)
	{

	}

	InstancedMesh::InstancedMesh(const VertexArrayRef& vertexArray, const uint32 maxInstances)
		: VertexArray(vertexArray)
		, IndexBuffer(vertexArray->GetIndexBuffer())
		, InstanceBufferObject(maxInstances)
	{
		if (VertexArray)
			VertexArray->AddVertexBuffer(InstanceBufferObject.GetVertexBuffer());
	}

	InstancedMesh::~InstancedMesh()
	{

	}

	void InstancedMesh::Renderer_Bind() const
	{
		VertexArray->Bind();
		IndexBuffer->Bind();
	}

	void InstancedMesh::Renderer_Unbind() const
	{
		VertexArray->Unbind();
		IndexBuffer->Unbind();
	}

	Ref<InstancedMesh> InstancedMesh::Create(const MeshData& data, const uint32 instances)
	{
		const std::vector<Vertex>& vertices = data.GetVertices();
		const std::vector<uint32>& indices = data.GetIndices();

		VertexArrayRef vao = VertexArray::Create();
		VertexBufferRef vbo = VertexBuffer::Create(vertices.data(), sizeof(Vertex) * static_cast<uint32>(vertices.size()));
		const IndexBufferRef ibo = IndexBuffer::Create(indices.data(), static_cast<uint32>(indices.size()));

		static const BufferLayout layout = {
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float2 },
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float3 }
		};

		vbo->SetLayout(layout);

		vao->AddVertexBuffer(vbo);
		vao->SetIndexBuffer(ibo);

		return new InstancedMesh(vao, instances);
	}

}