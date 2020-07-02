#pragma once

#include <glm/glm.hpp>

#include "PBR/Core/BaseTypes.h"
#include "PBR/Core/RefCounting.h"

#include "Context.h"
#include "VertexArray.h"



namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RHIImmediateCommandList /////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class RHICommandListImmediate
	{

	public:

		RHICommandListImmediate();
		~RHICommandListImmediate();

		void Clear(const ClearFlags flags = ClearFlags::ColorBuffer | ClearFlags::DepthBuffer);
		void SetClearColor(const glm::vec4& color);

		void SetDepthFunc(const DepthFunc func);
		void DepthMask(const bool value);
		void SetCullMode(const CullMode mode);

		void BeginRenderPass();
		void EndRenderPass();

		void Draw(const VertexArrayRef& vertexArray);
		void DrawIndexed(const VertexArrayRef& vertexArray);
		void DrawIndexedInstanced(const VertexArrayRef& vertexArray, const uint32 instanceCount);

	private:

		inline RHIContext& GetContext() { return RHIContext::GetContext(); }

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RHICommandListExecutor //////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class RHICommandListExecutor
	{

	public:

		static RHICommandListImmediate& GetImmediateCommandList() { return CommandListImmediate; }

	private:

		static RHICommandListImmediate CommandListImmediate;

	};

}