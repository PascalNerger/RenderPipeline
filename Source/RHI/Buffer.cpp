#include "pch.h"

#include "PBR/Renderer/Renderer.h"
#include "PBR/RHIOpenGL/OpenGLBuffer.h"

namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// VertexBuffer ////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Ref<VertexBuffer> VertexBuffer::Create(const void* data, const uint32 size, const BufferUsage usage)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLVertexBuffer(data, size, usage);

		default:
			break;
		}

		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::CreateMapped(const uint32 size, const BufferUsage usage)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLMappedVertexBuffer(size, usage);

		default:
			break;
		}

		return nullptr;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// IndexBuffer /////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Ref<IndexBuffer> IndexBuffer::Create(const uint32* data, const uint32 count, const BufferUsage usage)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLIndexBuffer(data, count, usage);

		default:
			break;
		}

		return nullptr;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// UniformBuffer ///////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Ref<UniformBuffer> UniformBuffer::Create(const uint32 size, const BufferUsage usage)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLUniformBuffer(size, usage);

		default:
			break;
		}

		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(const void* data, const uint32 size, const BufferUsage usage)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLUniformBuffer(data, size, usage);

		default:
			break;
		}

		return nullptr;
	}

}