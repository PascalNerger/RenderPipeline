#pragma once

#include "PBR/RenderCore/RenderStage.h"
#include "PBR/RenderCore/RenderPass.h"


namespace EngineCore
{
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Forward Declarations ////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Renderer;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ToneMapperPass //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	// Corresponds directly to the shader, do not change!
	enum class ToneMapper : int32
	{

		None		= 0,
		Reinhard	= 1,
		Exposure	= 2,
		Aces		= 3,
		GodotFilmic = 4,
		GodotAces	= 5

	};

	struct ToneMapperPassUniformStruct
	{

		alignas(4) int32 Type;
		alignas(4) float Exposure;

		alignas(8) uint64 HdrSourceTexture;

	};

	static_assert(sizeof(ToneMapperPassUniformStruct) == 16, "Wrong Uniform Buffer size: ToneMapperPassUniformStruct!");
	static_assert(offsetof(ToneMapperPassUniformStruct, Type) == 0, "Wrong Uniform Buffer offset: ToneMapperPassUniformStruct.Type!");
	static_assert(offsetof(ToneMapperPassUniformStruct, Exposure) == 4, "Wrong Uniform Buffer offset: ToneMapperPassUniformStruct.Exposure!");
	static_assert(offsetof(ToneMapperPassUniformStruct, HdrSourceTexture) == 8, "Wrong Uniform Buffer offset: ToneMapperPassUniformStruct.HdrSourceTexture!");

	class ToneMapperPass : public RenderPass
	{

	public:

		ToneMapperPass(Renderer* renderer);
		virtual ~ToneMapperPass();

		void Init();
		void Execute(const Texture2DRef sourceTexture);

	private:

		UniformBufferRef UniformBuffer;
		ToneMapperPassUniformStruct Uniforms;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ToneMapperStage /////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class ToneMapperStage : public RenderStage
	{

	public:

		ToneMapperStage(Renderer* renderer);
		virtual ~ToneMapperStage();

		void Init();
		void Execute(const Texture2DRef& sourceTexture);

		inline const Texture2DRef& GetToneMappedTexture() const { return Texture; }

	private:

		ToneMapperPass* ToneMapperPass;

		ShaderRef ToneMapperShader;
		FrameBufferRef ToneMapperFrameBuffer;

		Texture2DRef Texture;

	};

}