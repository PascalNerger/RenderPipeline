#pragma once

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
	// ScenePass ///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct SceneUniformStruct
	{

		alignas(16) glm::fvec3	CameraPosition;
		alignas(16) glm::fmat4	ViewMatrix;
		alignas(16) glm::fmat4	ProjectionMatrix;

		alignas(16) glm::fmat4	ModelMatrix;
		alignas(16) float		UseInstancing;

	};

	static_assert(sizeof(SceneUniformStruct) == 224, "Wrong Uniform Buffer size: SceneUniformStruct!");
	static_assert(offsetof(SceneUniformStruct, CameraPosition) == 0, "Wrong Uniform Buffer offset: SceneUniformStruct.CameraPosition!");
	static_assert(offsetof(SceneUniformStruct, ViewMatrix) == 16, "Wrong Uniform Buffer offset: SceneUniformStruct.ViewMatrix!");
	static_assert(offsetof(SceneUniformStruct, ProjectionMatrix) == 80, "Wrong Uniform Buffer offset: SceneUniformStruct.ProjectionMatrix!");
	static_assert(offsetof(SceneUniformStruct, ModelMatrix) == 144, "Wrong Uniform Buffer offset: SceneUniformStruct.ModelMatrix!");
	static_assert(offsetof(SceneUniformStruct, UseInstancing) == 208, "Wrong Uniform Buffer offset: SceneUniformStruct.UseInstancing!");

	struct ImageUniformStruct
	{

		alignas(4)	float		UseImageBasedLighting;

		alignas(8)	uint64		IrradianceMap;
		alignas(8)	uint64		PreFilteredMap;
		alignas(8)	uint64		BrdfTexture;

	};

	static_assert(sizeof(ImageUniformStruct) <= 48, "Wrong Uniform Buffer size: ImageUniformStruct!");
	static_assert(offsetof(ImageUniformStruct, UseImageBasedLighting) == 0, "Wrong Uniform Buffer offset: ImageUniformStruct.UseImageBasedLighting!");
	static_assert(offsetof(ImageUniformStruct, IrradianceMap) == 8, "Wrong Uniform Buffer offset: ImageUniformStruct.IrradianceMap!");
	static_assert(offsetof(ImageUniformStruct, PreFilteredMap) == 16, "Wrong Uniform Buffer offset: ImageUniformStruct.PreFilteredMap!");
	static_assert(offsetof(ImageUniformStruct, BrdfTexture) == 24, "Wrong Uniform Buffer offset: ImageUniformStruct.BrdfTexture!");

	struct ShadowUniformStruct
	{

		float UseShadowMap;
		float ShadowBias;

		uint64 ShadowMap;

		glm::fmat4 LightViewMatrix;
		glm::fmat4 LightProjectionMatrix;

	};

	static_assert(sizeof(ShadowUniformStruct) <= 144, "Wrong Uniform Buffer size: ShadowUniformStruct!");
	static_assert(offsetof(ShadowUniformStruct, UseShadowMap) == 0, "Wrong Uniform Buffer offset: ShadowUniformStruct.UseShadowMap!");
	static_assert(offsetof(ShadowUniformStruct, ShadowBias) == 4, "Wrong Uniform Buffer offset: ShadowUniformStruct.ShadowBias!");
	static_assert(offsetof(ShadowUniformStruct, ShadowMap) == 8, "Wrong Uniform Buffer offset: ShadowUniformStruct.ShadowMap!");
	static_assert(offsetof(ShadowUniformStruct, LightViewMatrix) == 16, "Wrong Uniform Buffer offset: ShadowUniformStruct.LightViewMatrix!");
	static_assert(offsetof(ShadowUniformStruct, LightProjectionMatrix) == 80, "Wrong Uniform Buffer offset: ShadowUniformStruct.LightProjectionMatrix!");

	class ScenePass : public RenderPass
	{

	public:

		ScenePass(Renderer* renderer);
		virtual ~ScenePass();

		void Init();
		void Execute(const RenderView& view, const TextureCubeRef& irradianceMap, const TextureCubeRef& preFilteredMap, const Texture2DRef& brdfTexture, const Texture2DRef& shadowMap);

	private:

		void SetSceneUniforms(const RenderView& view, const Transform& model, const bool instanced);
		void SetLightUniforms(const RenderView& view, const glm::vec3& point);
		void SetMaterialUniforms(const MaterialRef& material);
		void SetImageUniforms(const TextureCubeRef& irradianceMap, const TextureCubeRef& preFilteredMap, const Texture2DRef& brdfTexture);
		void SetShadowUniforms(const Texture2DRef& shadowMap, const SunRef& sun);

	private:

		UniformBufferRef SceneBuffer;
		UniformBufferRef LightBuffer;
		UniformBufferRef MaterialBuffer;
		UniformBufferRef ImageBuffer;
		UniformBufferRef ShadowBuffer;

		SceneUniformStruct SceneUniforms;
		ImageUniformStruct ImageUniforms;
		ShadowUniformStruct ShadowUniforms;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SceneStage //////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class SceneStage : public RenderStage
	{

	public:

		SceneStage(Renderer* renderer);
		virtual ~SceneStage();

		void Init();
		void Execute(const RenderView& view, const TextureCubeRef& irradianceMap, const TextureCubeRef& preFilteredMap, const Texture2DRef& brdfTexture, const Texture2DRef& shadowMap);

		inline const Texture2DRef& GetSceneTexture() const { return TargetTexture; }

	private:

		ScenePass* ScenePass;

		ShaderRef CommonShader;
		FrameBufferRef CommonFrameBuffer;

		Texture2DRef TargetTexture;

	private:

		friend class SkyboxStage;

	};

}