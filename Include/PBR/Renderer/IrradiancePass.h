#pragma once

#include <glm/glm.hpp>

#include "PBR/RHI/Shader.h"
#include "PBR/RHI/Texture.h"
#include "PBR/RHI/FrameBuffer.h"

#include "PBR/Engine/Mesh.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Forward Declarations ////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Renderer;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// IrradiancePass //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct IrradiancePassUniformStruct
	{

		alignas(16) glm::fmat4 ViewMatrix;
		alignas(16) glm::fmat4 ProjectionMatrix;

		alignas(16) uint64 CubeMap;

	};

	class IrradiancePass : public RenderPass
	{

	public:

		IrradiancePass(Renderer* renderer);
		~IrradiancePass();

		void Init();
		void Execute(const TextureCubeRef& cubeMap);

		inline void SetCubeRenderTarget(const TextureCubeRef& cubeRenderTarget) { CubeRenderTarget = cubeRenderTarget; }

	private:

		TextureCubeRef CubeRenderTarget;

		UniformBufferRef UniformBuffer;
		IrradiancePassUniformStruct Uniforms;

		glm::mat4 CaptureProjection;
		glm::mat4 CaptureViewMatrices[6];

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// IrradianceStage /////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class IrradianceStage : public RenderStage
	{

	public:

		IrradianceStage(Renderer* renderer);
		virtual ~IrradianceStage();

		void Init();
		void Execute(const TextureCubeRef& environmentMap);

		inline const TextureCubeRef& GetIrradianceMap() const { return IrradianceMap; }

	private:

		IrradiancePass* IrradiancePass;

		ShaderRef IrradianceShader;
		FrameBufferRef IrradianceFrameBuffer;

		TextureCubeRef IrradianceMap;

	};


}