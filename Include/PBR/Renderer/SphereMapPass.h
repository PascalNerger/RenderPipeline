#pragma once

#include "PBR/RHI/Texture.h"
#include "PBR/RHI/FrameBuffer.h"

#include "PBR/RenderCore/RenderStage.h"
#include "PBR/RenderCore/RenderPass.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Forward Declarations ////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Renderer;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SphereMapPass ///////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct SphereMapPassUniformStruct
	{

		alignas(64) glm::fmat4 ViewMatrix;
		alignas(64) glm::fmat4 ProjectionMatrix;

		alignas(8)	uint64 SphereMap;

	};

	class SphereMapPass : public RenderPass
	{

	public:

		SphereMapPass(Renderer* renderer);
		~SphereMapPass();

		void Init();
		void Execute(const Texture2DRef& sphereMap);

		void SetCubeRenderTarget(const TextureCubeRef& targetCube) { CubeRenderTarget = targetCube; }

	private:

		TextureCubeRef CubeRenderTarget;

		UniformBufferRef UniformBuffer;
		SphereMapPassUniformStruct Uniforms;

		glm::mat4 CaptureProjection;
		glm::mat4 CaptureViewMatrices[6];

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SphereMapStage //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class SphereMapStage : public RenderStage
	{

	public:

		SphereMapStage(Renderer* renderer);
		virtual ~SphereMapStage();

		void Init();
		void Execute(const Texture2DRef& sphereMap);

		inline const TextureCubeRef& GetCubeMap() const { return CubeMap; }

	private:

		SphereMapPass* SpherePass;

		ShaderRef SphereShader;
		FrameBufferRef SphereFrameBuffer;

		TextureCubeRef CubeMap;

	};

}