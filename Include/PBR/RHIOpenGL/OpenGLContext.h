#pragma once

#include "PBR/RHI/Context.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLContextState //////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLContextState
	{

	public:

		OpenGLContextState();
		~OpenGLContextState();

	public:

		uint32 BoundVertexArray;
		uint32 BoundVertexBuffer;
		uint32 BoundIndexBuffer;

	private:

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLRHIState //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLRHIState
	{

	public:

		OpenGLRHIState();
		~OpenGLRHIState();

		void Reset();

	public:

		CullMode CullMode;
		DepthFunc DepthFunc;
		BlendFunc BlendFunc;

		ShaderRef BoundShader;
		FrameBufferRef BoundFrameBuffer;

		VertexArrayRef BoundVertexArray;
		IndexBufferRef BoundIndexBuffer;

		UniformBufferRef BoundUniformBuffers[10];

	private:

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLContext ///////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLContext : public RHIContext
	{

	public:

		OpenGLContext();
		virtual ~OpenGLContext();

		virtual void RHIClear(const ClearFlags flags = ClearFlags::ColorBuffer | ClearFlags::DepthBuffer) final override;
		virtual void RHISetClearColor(const glm::vec4& color) final override;

		virtual void RHISetDepthFunc(const DepthFunc func) final override;
		virtual void RHIDepthMask(const bool value) final override;
		virtual void RHISetCullMode(const CullMode mode) final override;
		virtual void RHISetBlendFunc(const BlendFunc func) final override;

		virtual void RHISetViewport(const uint32 x, const uint32 y, const uint32 width, const uint32 height) final override;


		virtual void RHISetVertexBuffer(const VertexBufferRef& vertexBuffer) final override;
		virtual void RHISetIndexBuffer(const IndexBufferRef& indexBuffer) final override;
		virtual void RHISetVertexArray(const VertexArrayRef& vertexArray) final override;

		virtual void RHISetShader(const ShaderRef& shader) final override;
		virtual void RHISetFrameBuffer(const FrameBufferRef& frameBuffer) final override;

		virtual void RHISetShaderParameter() final override;
		virtual void RHISetShaderUniformParameter() final override;

		virtual void RHIBeginRenderPass() final override;
		virtual void RHIEndRenderPass() final override;

		virtual void RHIDrawPrimitive() final override;
		virtual void RHIDrawIndexedPrimitive() final override;
		virtual void RHIDrawIndexedInstancedPrimitive(const uint32 instances) final override;

	private:

		void CachedBindVertexArray(OpenGLContextState& context, const uint32 vertexArray);
		void CachedBindVertexBuffer(OpenGLContextState& context, const uint32 vertexBuffer);
		void CachedBindIndexBuffer(OpenGLContextState& context, const uint32 indexBuffer);

		virtual RenderApi GetApi() const final override { return RenderApi::OpenGL; }

	private:

		OpenGLRHIState PendingState;
		OpenGLContextState ContextState;

	};

}