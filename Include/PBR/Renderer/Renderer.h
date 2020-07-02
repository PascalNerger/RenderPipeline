#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "PBR/Engine/Mesh.h"
#include "PBR/Engine/Camera.h"
#include "PBR/Engine/Light.h"
#include "PBR/Engine/Sun.h"

#include "PBR/RHI/CommandList.h"
#include "PBR/RenderCore/RenderQueue.h"

#include "PBR/Renderer/ShadowPass.h"
#include "PBR/Renderer/ScenePass.h"
#include "PBR/Renderer/SkyboxPass.h"
#include "PBR/Renderer/Bloom.h"
#include "PBR/Renderer/ToneMapper.h"
#include "PBR/Renderer/Fxaa.h"
#include "PBR/Renderer/QuadPass.h"

#include "PBR/Renderer/IrradiancePass.h"
#include "PBR/Renderer/EnvPreFilterPass.h"
#include "PBR/Renderer/Brdf.h"

#include "PBR/Renderer/SphereMapPass.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Renderer ////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Renderer
	{

	public:

		Renderer();
		~Renderer();

		void Init();

		void DrawQuad();
		void DrawCube();
		void DrawMesh(const MeshRef& mesh);
		void DrawInstancedMesh(const InstancedMeshRef& mesh);

		void Submit(const RenderPrimitive& primitive);
		void Submit(const InstancedPrimitive& primitive);
		void AddLight(const LightRef& light);
		void SetSun(const SunRef& sun);

		void DrawScene();
		void DrawTexture(const Texture2DRef& texture);

		void PreComputeIndirectLighting(const TextureCubeRef& environmentMap);
		const TextureCubeRef& DrawSphereMapToCubeMap(const Texture2DRef& sphereMap);

		void Clear();

		inline void SetCamera(const CameraRef& camera) { Camera = camera; }
		inline void SetSkybox(const TextureCubeRef& skybox) { Skybox = skybox; }

		inline void SetShadowsEnabled(const bool value) { bIsShadowsEnabled = value; }
		inline void SetSkyboxEnabled(const bool value) { bIsSkyboxEnabled = value; }
		inline void SetBloomEnabled(const bool value) { bIsBloomEnabled = value; }
		inline void SetToneMappingEnabled(const bool value) { bIsToneMappingEnabled = value; }
		inline void SetFxaaEnabled(const bool value) { bIsFxaaEnabled = value; }


		inline RHICommandListImmediate& GetCommandList() { return CommandList; }

		inline ShadowStage* GetShadowStage() { return ShadowStage; }
		inline SceneStage* GetSceneStage() { return SceneStage; }
		inline SkyboxStage* GetSkyboxStage() { return SkyboxStage; }
		inline BloomStage* GetBloomStage() { return BloomStage; }
		inline ToneMapperStage* GetToneMapperStage() { return ToneMapper; }

		inline IrradianceStage* GetIrradianceStage() { return IrradianceStage; }
		inline EnvPreFilterStage* GetEnvPreFilterStage() { return PreFilterStage; }
		inline BrdfStage* GetBrdfStage() { return BrdfStage; }

	private:

		RenderView CreateRenderView(const RenderQueue& queue);

		MeshRef CreateQuadMesh() const;
		MeshRef CreateCubeMesh() const;
		MeshRef CreateSphereMesh(uint32 segments) const;

	public:

		static RenderApi GetApi() { return RHIContext::GetContext().GetApi(); }

	private:

		static Renderer* Instance;

		RHICommandListImmediate CommandList;

		RenderQueue Queue;
		std::vector<LightRef> Lights;
		std::vector<InstancedPrimitive> InstancedPrimitives;

		SunRef Sun;

		ShadowStage* ShadowStage;
		SceneStage* SceneStage;
		SkyboxStage* SkyboxStage;
		BloomStage* BloomStage;
		ToneMapperStage* ToneMapper;
		FxaaStage* FxaaStage;
		QuadStage* QuadStage;

		IrradianceStage* IrradianceStage;
		EnvPreFilterStage* PreFilterStage;
		BrdfStage* BrdfStage;

		SphereMapStage* SphereMapStage;

		CameraRef Camera;
		TextureCubeRef Skybox;

		MeshRef QuadMesh;
		MeshRef CubeMesh;

		bool bIsShadowsEnabled;
		bool bIsSkyboxEnabled;
		bool bIsBloomEnabled;
		bool bIsToneMappingEnabled;
		bool bIsFxaaEnabled;

	};


}