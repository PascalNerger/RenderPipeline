#pragma once

#include <vector>

#include "PBR/Core/BaseTypes.h"
#include "PBR/Core/RefCounting.h"

#include "Buffer.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// VertexArray /////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct VertexArray : public RefCountedObject
	{

		virtual ~VertexArray() { }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& buffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& buffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;


		static Ref<VertexArray> Create();

	};

	using VertexArrayRef = Ref<VertexArray>;

}