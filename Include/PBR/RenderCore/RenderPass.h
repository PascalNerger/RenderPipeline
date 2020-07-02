#pragma once

#include "PBR/Core/RefCounting.h"

#include "PBR/RHI/FrameBuffer.h"
#include "PBR/RHI/CommandList.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Forward Declarations ////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Renderer;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// RenderPass //////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class RenderPass
	{

	public:

		RenderPass(Renderer* renderer)
			: RendererInstance(renderer)
			, Primitive(PrimitiveType::Triangles)
			, CullingMode(CullMode::Back)
			, BlendingFunc(BlendFunc::OneMinusSourceAlpha)
			, Shader(nullptr)
			, FrameBuffer(nullptr)
			, RenderTarget(nullptr)
			, DepthTarget(nullptr)
			, StencilTarget(nullptr)
		{

		}

		virtual ~RenderPass()
		{

		}

		inline void SetPrimitiveType(const PrimitiveType primitive) { Primitive = primitive; }
		inline void SetCullMode(const CullMode mode) { CullingMode = mode; }
		inline void SetBlendFunc(const BlendFunc func) { BlendingFunc = func; }

		inline void SetShader(const ShaderRef& shader) { Shader = shader; }
		inline void SetFrameBuffer(const FrameBufferRef& frameBuffer) { FrameBuffer = frameBuffer; }

		inline void SetRenderTarget(const Texture2DRef& target) { RenderTarget = target; }
		inline void SetDepthTarget(const Texture2DRef& target) { DepthTarget = target; }
		inline void SetStencilTarget(const Texture2DRef& target) { StencilTarget = target; }


		inline const ShaderRef& GetShader() const { return Shader; }
		inline const FrameBufferRef& GetFrameBuffer() const { return FrameBuffer; }

		inline const Texture2DRef& GetRenderTarget() const { return RenderTarget; }
		inline const Texture2DRef& GetDepthTarget() const { return DepthTarget; }
		inline const Texture2DRef& GetStencilTarget() const { return StencilTarget; }

	protected:

		Renderer* RendererInstance;

		PrimitiveType Primitive;
		CullMode CullingMode;
		BlendFunc BlendingFunc;

		ShaderRef Shader;
		FrameBufferRef FrameBuffer;

		Texture2DRef RenderTarget;
		Texture2DRef DepthTarget;
		Texture2DRef StencilTarget;

	};

}