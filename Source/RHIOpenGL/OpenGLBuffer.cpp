#include "pch.h"

#include "PBR/RHIOpenGL/OpenGL.h"
#include "PBR/RHIOpenGL/OpenGLBuffer.h"

namespace EngineCore
{

	GLenum BufferUsageToGl(const BufferUsage usage)
	{
		switch (usage)
		{
		case BufferUsage::Static:	return GL_STATIC_DRAW;
		case BufferUsage::Dynamic:	return GL_DYNAMIC_DRAW;
		case BufferUsage::Stream:	return GL_STREAM_DRAW;

		default: break;
		}

		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLVertexBuffer //////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, const uint32 size, const BufferUsage usage)
		: Handle(0), Layout()
	{
		glGenBuffers(1, &Handle);
		glBindBuffer(GL_ARRAY_BUFFER, Handle);
		glBufferData(GL_ARRAY_BUFFER, size, data, BufferUsageToGl(usage));
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &Handle);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, Handle);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, const uint32 offset, const uint32 size)
	{
		// TODO:
		assert(false);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLMappedVertexBuffer ////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLMappedVertexBuffer::OpenGLMappedVertexBuffer(const uint32 size, const BufferUsage usage)
		: Handle(0), Layout(), MappedMemory(nullptr)
	{
		glGenBuffers(1, &Handle);
		glBindBuffer(GL_ARRAY_BUFFER, Handle);

		uint32 flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

		glBufferStorage(GL_ARRAY_BUFFER, size, nullptr, flags);
		MappedMemory = glMapNamedBufferRange(Handle, 0, size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLMappedVertexBuffer::SetData(const void* data, const uint32 offset, const uint32 size)
	{
		std::memcpy(static_cast<byte*>(MappedMemory) + offset, data, size);
	}

	OpenGLMappedVertexBuffer::~OpenGLMappedVertexBuffer()
	{
		glUnmapNamedBuffer(Handle); // TODO: generates error
		glDeleteBuffers(1, &Handle);
	}

	void OpenGLMappedVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, Handle);
	}

	void OpenGLMappedVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLIndexBuffer ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32* data, const uint32 count, const BufferUsage usage)
		: Handle(0), Count(count)
	{
		glGenBuffers(1, &Handle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32), data, BufferUsageToGl(usage));
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &Handle);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Handle);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLUniformBuffer /////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLUniformBuffer::OpenGLUniformBuffer(const uint32 size, const BufferUsage usage)
		: Handle(0), Size(size), Usage(usage)
	{
		glGenBuffers(1, &Handle);

		glBindBuffer(GL_UNIFORM_BUFFER, Handle);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, BufferUsageToGl(Usage));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, const uint32 size, const BufferUsage usage)
		: Handle(0), Size(size), Usage(usage)
	{
		glGenBuffers(1, &Handle);
		
		glBindBuffer(GL_UNIFORM_BUFFER, Handle);
		glBufferData(GL_UNIFORM_BUFFER, size, data, BufferUsageToGl(Usage));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &Handle);
	}

	void OpenGLUniformBuffer::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, Handle);
	}

	void OpenGLUniformBuffer::Unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::BindBlock(const uint32 blockIndex) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, Handle);
	}

	void OpenGLUniformBuffer::BindBlockRange(const uint32 blockIndex, const uint32 offset, const uint32 size) const
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, blockIndex, Handle, offset, size);
	}

	void OpenGLUniformBuffer::SetData(const void* data, const uint32 size, const uint32 offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, Handle);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

}