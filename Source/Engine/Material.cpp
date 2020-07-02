#include "pch.h"

#include "PBR/Engine/Material.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Material ////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Material::Material()
		: Albedo()
		, Metalness(0.0f)
		, Roughness(0.0f)
		, AmbientOcclusion(0.0f)
		, DisplacementHeightScale(0.0f)

		, UseAlbedoMap(false)
		, UseMetalnessMap(false)
		, UseRoughnessMap(false)
		, UseAmbientOcclusionMap(false)
		, UseNormalMap(false)
		, UseDisplacementMap(false)

		, AlbedoMap(0)
		, MetalnessMap(0)
		, RoughnessMap(0)
		, AmbientOcclusionMap(0)
		, NormalMap(0)
		, DisplacementMap(0)
	{

	}

	Material::~Material()
	{

	}

	void Material::GetShaderParameters(MaterialUniformStruct& material) const
	{
		material.Albedo = Albedo;
		material.UseAlbedoMap = UseAlbedoMap ? 1.0f : 0.0f;

		material.Metalness = Metalness;
		material.UseMetalnessMap = UseMetalnessMap ? 1.0f : 0.0f;

		material.Roughness = Roughness;
		material.UseRoughnessMap = UseRoughnessMap ? 1.0f : 0.0f;

		material.AmbientOcclusion = AmbientOcclusion;
		material.UseAmbientOcclusionMap = UseAmbientOcclusionMap ? 1.0f : 0.0f;

		material.UseNormalMap = UseNormalMap ? 1.0f : 0.0f;

		material.UseDisplacementMap = UseDisplacementMap ? 1.0f : 0.0f;
		material.DisplacementHeightScale = DisplacementHeightScale;

		material.AlbedoMap = AlbedoMap ? AlbedoMap->GetShaderHandle() : 0;
		material.MetalnessMap = MetalnessMap ? MetalnessMap->GetShaderHandle() : 0;
		material.RoughnessMap = RoughnessMap ? RoughnessMap->GetShaderHandle() : 0;
		material.AmbientOcclusionMap = AmbientOcclusionMap ? AmbientOcclusionMap->GetShaderHandle() : 0;
		material.NormalMap = NormalMap ? NormalMap->GetShaderHandle() : 0;
		material.DisplacementMap = DisplacementMap ? DisplacementMap->GetShaderHandle() : 0;
	}

}