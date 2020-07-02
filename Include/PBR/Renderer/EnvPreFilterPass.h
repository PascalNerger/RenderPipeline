#pragma once

#include <array>
#include <glm/glm.hpp>

#include "PBR/RHI/Shader.h"
#include "PBR/RHI/Texture.h"
#include "PBR/RHI/FrameBuffer.h"

#include "PBR/RenderCore/RenderPass.h"
#include "PBR/RenderCore/RenderStage.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Forward Declarations ////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Renderer;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// EnvPreFilterPass ////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct EnvPreFilterUniformStruct
	{

		alignas(16) glm::fmat4 ViewMatrix;
		alignas(16) glm::fmat4 ProjectionMatrix;

		alignas(4) float Roughness;
		alignas(4) float MapResolution;

		alignas(8) uint64 EnvironmentMap;

	};

	static_assert(sizeof(EnvPreFilterUniformStruct) <= 144, "Wrong Uniform Buffer size: EnvPreFilterUniformStruct!");
	static_assert(offsetof(EnvPreFilterUniformStruct, ViewMatrix) == 0, "Wrong Uniform Buffer offset: EnvPreFilterUniformStruct.ViewMatrix!");
	static_assert(offsetof(EnvPreFilterUniformStruct, ProjectionMatrix) == 64, "Wrong Uniform Buffer offset: EnvPreFilterUniformStruct.ProjectionMatrix!");
	static_assert(offsetof(EnvPreFilterUniformStruct, Roughness) == 128, "Wrong Uniform Buffer offset: EnvPreFilterUniformStruct.Roughness!");
	static_assert(offsetof(EnvPreFilterUniformStruct, MapResolution) == 132, "Wrong Uniform Buffer offset: EnvPreFilterUniformStruct.MapResolution!");
	static_assert(offsetof(EnvPreFilterUniformStruct, EnvironmentMap) == 136, "Wrong Uniform Buffer offset: EnvPreFilterUniformStruct.EnvironmentMap!");

	class EnvPreFilterPass : public RenderPass
	{

	public:

		EnvPreFilterPass(Renderer* renderer);
		virtual ~EnvPreFilterPass();

		void Init();
		void Execute(const TextureCubeRef& environmentMap);

		inline void SetCubeRenderTarget(const TextureCubeRef& cubeRenderTarget) { CubeRenderTarget = cubeRenderTarget; }

	private:

		TextureCubeRef CubeRenderTarget;

		UniformBufferRef UniformBuffer;
		EnvPreFilterUniformStruct Uniforms;

		glm::mat4 CaptureProjection;
		std::array<glm::mat4, 6> CaptureViewMatrices;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// EnvPreFilterStage ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class EnvPreFilterStage : public RenderStage
	{

	public:

		EnvPreFilterStage(Renderer* renderer);
		virtual ~EnvPreFilterStage();

		void Init();
		void Execute(const TextureCubeRef& environmentMap);

		inline const TextureCubeRef& GetPreFilteredMap() const { return PreFilterCube; }

	private:

		EnvPreFilterPass* PreFilterPass;

		ShaderRef PreFilterShader;
		FrameBufferRef PreFilterFrameBuffer;

		TextureCubeRef PreFilterCube;

	};


}