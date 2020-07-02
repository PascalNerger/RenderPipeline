#pragma once

#include <vector>

#include "PBR/Core/BaseTypes.h"
#include "PBR/RHI/VertexArray.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLVertexArray ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLVertexArray : public VertexArray
	{

	private:

		using VertexBufferRHIList = std::vector<VertexBufferRef>;

	public:

		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const VertexBufferRef& buffer) override;
		virtual void SetIndexBuffer(const IndexBufferRef& buffer) override;

		virtual const VertexBufferRHIList& GetVertexBuffers() const override { return VertexBuffers; }
		virtual const IndexBufferRef& GetIndexBuffer() const override { return IndexBuffer; }

	private:

		uint32 Handle;
		uint32 CurrentVertexAttribPointer;

		VertexBufferRHIList VertexBuffers;
		IndexBufferRef IndexBuffer;

	};

}