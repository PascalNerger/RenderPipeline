#pragma once

#include <glm/glm.hpp>

#include "PBR/Core/BaseTypes.h"
#include "PBR/Core/RefCounting.h"

#include "PBR/RHI/Texture.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Material ////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct MaterialUniformStruct
	{

		/* alignas(12) */	glm::fvec3 Albedo;
		alignas(4)			float UseAlbedoMap;

		alignas(4)			float Metalness;
		alignas(4)			float UseMetalnessMap;

		alignas(4)			float Roughness;
		alignas(4)			float UseRoughnessMap;

		alignas(4)			float AmbientOcclusion;
		alignas(4)			float UseAmbientOcclusionMap;

		alignas(4)			float UseNormalMap;

		alignas(4)			float UseDisplacementMap;
		alignas(4)			float DisplacementHeightScale;

		alignas(8)			uint64 AlbedoMap;
		alignas(8)			uint64 MetalnessMap;
		alignas(8)			uint64 RoughnessMap;
		alignas(8)			uint64 AmbientOcclusionMap;
		alignas(8)			uint64 NormalMap;
		alignas(8)			uint64 DisplacementMap;

	};

	class Material : public RefCountedObject
	{

	public:

		Material();
		virtual ~Material();

		void GetShaderParameters(MaterialUniformStruct& material) const;

		inline const glm::vec3& GetAlbedo() const { return Albedo; }
		inline float GetMetalness() const { return Metalness; }
		inline float GetRoughness() const { return Roughness; }
		inline float GetAmbientOcclusion() const { return AmbientOcclusion; }
		inline float GetDisplacementHeightScale() const { return DisplacementHeightScale; }

		inline bool GetUseAlbedoMap() const { return UseAlbedoMap; }
		inline bool GetUseMetalnessMap() const { return UseMetalnessMap; }
		inline bool GetUseRoughnessMap() const { return UseRoughnessMap; }
		inline bool GetUseAmbientOcclusionMap() const { return UseAmbientOcclusionMap; }
		inline bool GetUseNormalMap() const { return UseNormalMap; }
		inline bool GetUseDisplacementMap() const { return UseDisplacementMap; }

		inline const Texture2DRef& GetAlbedoMap() const { return AlbedoMap; }
		inline const Texture2DRef& GetMetalnessMap() const { return MetalnessMap; }
		inline const Texture2DRef& GetRoughnessMap() const { return RoughnessMap; }
		inline const Texture2DRef& GetAmbientOcclusionMap() const { return AmbientOcclusionMap; }
		inline const Texture2DRef& GetNormalMap() const { return NormalMap; }
		inline const Texture2DRef& GetDisplacementMap() const { return DisplacementMap; }


		inline void SetAlbedo(const glm::vec3& albedo) { Albedo = albedo; UseAlbedoMap = false; }
		inline void SetMetalness(const float metalness) { Metalness = metalness; UseMetalnessMap = false; }
		inline void SetRoughness(const float roughness) { Roughness = roughness; UseRoughnessMap = false; }
		inline void SetAmbientOcclusion(const float occlusion) { AmbientOcclusion = occlusion; UseAmbientOcclusionMap = false; }
		inline void SetDisplacementHeightScale(const float scale) { DisplacementHeightScale = scale; }

		inline void SetAlbedoMap(const Texture2DRef& texture) { AlbedoMap = texture; UseAlbedoMap = true; }
		inline void SetMetalnessMap(const Texture2DRef& texture) { MetalnessMap = texture; UseMetalnessMap = true; }
		inline void SetRoughnessMap(const Texture2DRef& texture) { RoughnessMap = texture; UseRoughnessMap = true; }
		inline void SetAmbientOcclusionMap(const Texture2DRef& texture) { AmbientOcclusionMap = texture;  UseAmbientOcclusionMap = true; }
		inline void SetNormalMap(const Texture2DRef& texture) { NormalMap = texture; UseNormalMap = true; }
		inline void SetDisplacementMap(const Texture2DRef& texture) { DisplacementMap = texture; UseDisplacementMap = true; }

	private:

		glm::vec3 Albedo;
		float Metalness;
		float Roughness;
		float AmbientOcclusion;
		float DisplacementHeightScale;

		bool UseAlbedoMap;
		bool UseMetalnessMap;
		bool UseRoughnessMap;
		bool UseAmbientOcclusionMap;
		bool UseNormalMap;
		bool UseDisplacementMap;

		Texture2DRef AlbedoMap;
		Texture2DRef MetalnessMap;
		Texture2DRef RoughnessMap;
		Texture2DRef AmbientOcclusionMap;
		Texture2DRef NormalMap;
		Texture2DRef DisplacementMap;

	};

	using MaterialRef = Ref<Material>;

}