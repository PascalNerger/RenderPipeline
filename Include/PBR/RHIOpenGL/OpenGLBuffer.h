#pragma once

#include "PBR/RHI/Buffer.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLVertexBuffer //////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLVertexBuffer : public VertexBuffer
	{

	public:

		OpenGLVertexBuffer(const void* data, const uint32 size, const BufferUsage usage = BufferUsage::Static);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, const uint32 offset, const uint32 size) override;

		virtual void SetLayout(const BufferLayout& layout) override { Layout = layout; }
		virtual const BufferLayout& GetLayout() const override { return Layout; }

	private:

		uint32 Handle;
		BufferLayout Layout;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLMappedVertexBuffer ////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLMappedVertexBuffer : public VertexBuffer
	{

	public:

		OpenGLMappedVertexBuffer(const uint32 size, const BufferUsage usage = BufferUsage::Dynamic);
		virtual ~OpenGLMappedVertexBuffer();

		virtual void Bind() const final override;
		virtual void Unbind() const final override;

		virtual void SetData(const void* data, const uint32 offset, const uint32 size) final override;

		virtual void SetLayout(const BufferLayout& layout) final override { Layout = layout; }
		virtual const BufferLayout& GetLayout() const final override { return Layout; }

	private:

		uint32 Handle;
		BufferLayout Layout;

		void* MappedMemory;
		uint32 Size;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLIndexBuffer ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLIndexBuffer : public IndexBuffer
	{

	public:

		OpenGLIndexBuffer(const uint32* data, const uint32 count, const BufferUsage usage = BufferUsage::Static);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32 GetCount() const override { return Count; }

	private:

		uint32 Handle;
		uint32 Count;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLUniformBuffer /////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLUniformBuffer : public UniformBuffer
	{

	public:

		OpenGLUniformBuffer(const uint32 size, const BufferUsage usage = BufferUsage::Static);
		OpenGLUniformBuffer(const void* data, const uint32 size, const BufferUsage usage = BufferUsage::Static);
		virtual ~OpenGLUniformBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void BindBlock(const uint32 blockIndex) const override;
		virtual void BindBlockRange(const uint32 blockIndex, const uint32 offset, const uint32 size) const override;

		virtual void SetData(const void* data, const uint32 size, const uint32 offset = 0) override;

		virtual uint32 GetSize() const { return Size; }


	private:

		uint32 Handle;
		uint32 Size;

		const BufferUsage Usage;

	};

}