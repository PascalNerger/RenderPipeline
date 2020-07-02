#include "pch.h"

#include <assert.h>

#include "PBR/RHI/Buffer.h"

#include "PBR/RHIOpenGL/OpenGL.h"
#include "PBR/RHIOpenGL/OpenGLVertexArray.h"



namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLVertexArray ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLVertexArray::OpenGLVertexArray()
		: Handle(0)
		, CurrentVertexAttribPointer(0)
		, VertexBuffers()
		, IndexBuffer()
	{
		glGenVertexArrays(1, &Handle);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &Handle);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(Handle);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	GLenum ShaderDataTypeToGlBaseType(const ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Int:		return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;

		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;

		case ShaderDataType::Mat3:		return GL_FLOAT;
		case ShaderDataType::Mat4:		return GL_FLOAT;

		case ShaderDataType::Bool:		return GL_BOOL;

		default: break;
		}

		return 0;
	}

	void OpenGLVertexArray::AddVertexBuffer(const VertexBufferRef& buffer)
	{
		assert(buffer->GetLayout().GetElements().size()); // "VertexBuffer has no layout."

		glBindVertexArray(Handle);
		buffer->Bind();

		uint32 offset = 0;
		const BufferLayout& layout = buffer->GetLayout();

		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(CurrentVertexAttribPointer);
			glVertexAttribPointer(
				CurrentVertexAttribPointer,
				element.GetComponentCount(),
				ShaderDataTypeToGlBaseType(element.Type),
				element.Normalized,
				layout.GetStride(),
				reinterpret_cast<const void*>(static_cast<uint64>(element.Offset)) // TODO: static_cast<uint64> only for x64 systems!
			);

			if (element.Divisor != -1)
				glVertexAttribDivisor(CurrentVertexAttribPointer, element.Divisor);

			offset += element.Offset;
			++CurrentVertexAttribPointer;
		}

		VertexBuffers.push_back(buffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const IndexBufferRef& buffer)
	{
		glBindVertexArray(Handle);
		buffer->Bind();

		IndexBuffer = buffer;
	}

}