#include "pch.h"

#include "PBR/RHI/CommandList.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RHIImmediateCommandList /////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	RHICommandListImmediate::RHICommandListImmediate()
	{

	}

	RHICommandListImmediate::~RHICommandListImmediate()
	{

	}

	void RHICommandListImmediate::Clear(const ClearFlags flags)
	{
		GetContext().RHIClear(flags);
	}

	void RHICommandListImmediate::SetClearColor(const glm::vec4& color)
	{
		GetContext().RHISetClearColor(color);
	}

	void RHICommandListImmediate::SetDepthFunc(const DepthFunc func)
	{
		GetContext().RHISetDepthFunc(func);
	}

	void RHICommandListImmediate::DepthMask(const bool value)
	{
		GetContext().RHIDepthMask(value);
	}

	void RHICommandListImmediate::SetCullMode(const CullMode mode)
	{
		GetContext().RHISetCullMode(mode);
	}

	void RHICommandListImmediate::BeginRenderPass()
	{
		GetContext().RHIBeginRenderPass();
	}

	void RHICommandListImmediate::EndRenderPass()
	{
		GetContext().RHIEndRenderPass();
	}

	void RHICommandListImmediate::Draw(const VertexArrayRef& vertexArray)
	{
		GetContext().RHISetVertexArray(vertexArray);
		GetContext().RHIDrawPrimitive();
	}

	void RHICommandListImmediate::DrawIndexed(const VertexArrayRef& vertexArray)
	{
		GetContext().RHISetVertexArray(vertexArray);
		GetContext().RHIDrawIndexedPrimitive();
	}

	void RHICommandListImmediate::DrawIndexedInstanced(const VertexArrayRef& vertexArray, const uint32 instanceCount)
	{
		GetContext().RHISetVertexArray(vertexArray);
		GetContext().RHIDrawIndexedInstancedPrimitive(instanceCount);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RHICommandListExecutor //////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	RHICommandListImmediate RHICommandListExecutor::CommandListImmediate = RHICommandListImmediate();

}