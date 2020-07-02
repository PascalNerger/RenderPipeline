#pragma once

#include <glm/glm.hpp>

#include "PBR/RHI/Buffer.h"
#include "PBR/RenderCore/RenderStage.h"
#include "PBR/RenderCore/RenderPass.h"
#include "PBR/RenderCore/RenderView.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Forward Declarations ////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Renderer;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ShadowPass //////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct ShadowPassUniformStruct
	{

		alignas(16) glm::fmat4 ModelMatrix;

		alignas(16) glm::fmat4 LightViewMatrix;
		alignas(16) glm::fmat4 LightProjectionMatrix;
		
		alignas(16) float UseInstancing;

	};

	static_assert(sizeof(ShadowPassUniformStruct) <= 208, "Wrong Uniform Buffer size: ShadowPassUniformStruct!");
	static_assert(offsetof(ShadowPassUniformStruct, ModelMatrix) == 0, "Wrong Uniform Buffer offset: ShadowPassUniformStruct.ModelMatrix!");
	static_assert(offsetof(ShadowPassUniformStruct, LightViewMatrix) == 64, "Wrong Uniform Buffer offset: ShadowPassUniformStruct.LightViewMatrix!");
	static_assert(offsetof(ShadowPassUniformStruct, LightProjectionMatrix) == 128, "Wrong Uniform Buffer offset: ShadowPassUniformStruct.LightProjectionMatrix!");
	static_assert(offsetof(ShadowPassUniformStruct, UseInstancing) == 192, "Wrong Uniform Buffer offset: ShadowPassUniformStruct.UseInstancing!");

	class ShadowPass : public RenderPass
	{

	public:

		ShadowPass(Renderer* renderer);
		virtual ~ShadowPass();

		void Init();
		void Execute(const RenderView& renderView);

	private:

		UniformBufferRef UniformBuffer;
		ShadowPassUniformStruct Uniforms;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ShadowStage /////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class ShadowStage : public RenderStage
	{

	public:

		ShadowStage(Renderer* renderer);
		virtual ~ShadowStage();

		void Init();
		void Execute(const RenderView& renderView);

		inline const Texture2DRef& GetDepthTexture() const { return DepthTexture; }

	private:

		ShadowPass* ShadowPass;

		ShaderRef ShadowShader;
		FrameBufferRef ShadowFrameBuffer;

		Texture2DRef DepthTexture;

	};

}