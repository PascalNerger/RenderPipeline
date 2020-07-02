#include "pch.h"

#include "PBR/RHIOpenGL/OpenGL.h"
#include "PBR/RHIOpenGL/OpenGLContext.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLContextState //////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLContextState::OpenGLContextState()
	{

	}

	OpenGLContextState::~OpenGLContextState()
	{

	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLRHIState //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLRHIState::OpenGLRHIState()
		: CullMode(CullMode::None)
		, DepthFunc(DepthFunc::None)
		, BlendFunc(BlendFunc::None)
		, BoundShader(nullptr)
		, BoundFrameBuffer(nullptr)
		, BoundVertexArray(nullptr)
	{

	}

	OpenGLRHIState::~OpenGLRHIState()
	{

	}

	void OpenGLRHIState::Reset()
	{
		BoundShader = nullptr;
		BoundFrameBuffer = nullptr;

		BoundVertexArray = nullptr;
		BoundIndexBuffer = nullptr;

		for (uint32 i = 0; i < 10; ++i)
			BoundUniformBuffers[i] = nullptr;

	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLContext ///////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLContext::OpenGLContext()
	{
	}

	OpenGLContext::~OpenGLContext()
	{

	}

	void OpenGLContext::RHIClear(const ClearFlags flags)
	{
		uint32 glFlags = 0;

		if ((flags & ClearFlags::ColorBuffer) != ClearFlags::None)
			glFlags |= GL_COLOR_BUFFER_BIT;

		if ((flags & ClearFlags::DepthBuffer) != ClearFlags::None)
			glFlags |= GL_DEPTH_BUFFER_BIT;

		if ((flags & ClearFlags::StencilBuffer) != ClearFlags::None)
			glFlags |= GL_STENCIL_BUFFER_BIT;

		glClear(glFlags);
	}

	void OpenGLContext::RHISetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLContext::RHISetDepthFunc(const DepthFunc func)
	{
		// TODO:
		//if (PendingState.DepthFunc == func)
		//	return;

		//bool isOldDepthTestActive = PendingState.DepthFunc != DepthFunc::None;
		//if (func != DepthFunc::None && !isOldDepthTestActive)
		//	glEnable(GL_DEPTH_TEST);
		//else if(func == DepthFunc::None && isOldDepthTestActive)
		//{
		//	glDisable(GL_DEPTH_TEST);
		//	PendingState.DepthFunc = func;

		//	return;
		//}

		//switch (func)
		//{
		//	case DepthFunc::Less:		glDepthFunc(GL_LESS);	break;
		//	case DepthFunc::LessEqual:	glDepthFunc(GL_LEQUAL); break;

		//	default: break;
		//}

		//PendingState.DepthFunc = func;

		switch (func)
		{
		case DepthFunc::None:		glDisable(GL_DEPTH_TEST);
		case DepthFunc::Less:		glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);	break;
		case DepthFunc::LessEqual:	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); break;

		default: break;
		}
	}

	void OpenGLContext::RHIDepthMask(const bool value)
	{
		glDepthMask(value ? GL_TRUE : GL_FALSE);
	}

	void OpenGLContext::RHISetCullMode(const CullMode mode)
	{
		// TODO:
		//if (PendingState.CullMode == mode)
		//	return;

		//bool isOldCullModeActive = PendingState.CullMode != CullMode::None;
		//if (mode != CullMode::None && !isOldCullModeActive)
		//	glEnable(GL_CULL_FACE);
		//else if(mode == CullMode::None && isOldCullModeActive)
		//{
		//	glDisable(GL_CULL_FACE);
		//	PendingState.CullMode = mode;

		//	return;
		//}

		//switch (mode)
		//{
		//	case CullMode::Front:	glCullFace(GL_FRONT);	break;
		//	case CullMode::Back:	glCullFace(GL_BACK);	break;

		//	default: break;
		//}

		//PendingState.CullMode = mode;

		switch (mode)
		{
		case CullMode::None:	glDisable(GL_CULL_FACE); break;
		case CullMode::Front:	glEnable(GL_CULL_FACE); glCullFace(GL_FRONT);	break;
		case CullMode::Back:	glEnable(GL_CULL_FACE); glCullFace(GL_BACK);	break;

		default: break;
		}
	}

	void OpenGLContext::RHISetBlendFunc(const BlendFunc func)
	{

	}

	void OpenGLContext::RHISetViewport(const uint32 x, const uint32 y, const uint32 width, const uint32 height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLContext::RHISetVertexBuffer(const VertexBufferRef& vertexBuffer)
	{

	}

	void OpenGLContext::RHISetIndexBuffer(const IndexBufferRef& indexBuffer)
	{

	}

	void OpenGLContext::RHISetVertexArray(const VertexArrayRef& vertexArray)
	{
		PendingState.BoundVertexArray = vertexArray;
	}

	void OpenGLContext::RHISetShader(const ShaderRef& shader)
	{
		PendingState.BoundShader = shader;
	}

	void OpenGLContext::RHISetFrameBuffer(const FrameBufferRef& frameBuffer)
	{
		PendingState.BoundFrameBuffer = frameBuffer;
	}

	void OpenGLContext::RHISetShaderParameter()
	{

	}

	void OpenGLContext::RHISetShaderUniformParameter()
	{

	}

	void OpenGLContext::RHIBeginRenderPass()
	{

	}

	void OpenGLContext::RHIEndRenderPass()
	{

	}

	void OpenGLContext::RHIDrawPrimitive()
	{
		//PendingState.BoundShader->Bind();

		//PendingState.BoundFrameBuffer->Bind();
		//PendingState.BoundFrameBuffer->Clear();
		//// AttachRenderTargets();

		//PendingState.BoundVertexArray->Bind();
		//PendingState.BoundVertexArray->GetIndexBuffer()->Bind();

		//// Draw

		//PendingState.BoundVertexArray->GetIndexBuffer->Unbind();
		//PendingState.BoundVertexArray->Unbind();

		//PendingState.BoundFrameBuffer->Unbind();

		//PendingState.BoundShader->Unbind();
	}

	void OpenGLContext::RHIDrawIndexedPrimitive()
	{
		const uint32 count = PendingState.BoundVertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLContext::RHIDrawIndexedInstancedPrimitive(const uint32 instances)
	{
		const uint32 count = PendingState.BoundVertexArray->GetIndexBuffer()->GetCount();
		glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instances);
	}

	void OpenGLContext::CachedBindVertexArray(OpenGLContextState& context, const uint32 vertexArray)
	{
		if (context.BoundVertexArray == vertexArray)
			return;

		glBindVertexArray(vertexArray);
		context.BoundVertexArray = vertexArray;
	}

	void OpenGLContext::CachedBindVertexBuffer(OpenGLContextState& context, const uint32 vertexBuffer)
	{
		if (context.BoundVertexBuffer == vertexBuffer)
			return;

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		context.BoundVertexBuffer = vertexBuffer;
	}

	void OpenGLContext::CachedBindIndexBuffer(OpenGLContextState& context, const uint32 indexBuffer)
	{
		if (context.BoundIndexBuffer == indexBuffer)
			return;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		context.BoundIndexBuffer = indexBuffer;
	}

}