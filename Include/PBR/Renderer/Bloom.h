#pragma once

#include "PBR/RHI/Shader.h"
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
	// BrightnessPass //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct BrightPassUniformStruct
	{

		alignas(8) uint64 SceneTexture;
		alignas(4) float Threshold;

	};

	static_assert(sizeof(BrightPassUniformStruct) <= 16, "Wrong Uniform Buffer size: BrightPassUniformStruct!");
	static_assert(offsetof(BrightPassUniformStruct, SceneTexture) == 0, "Wrong Uniform Buffer offset: BrightPassUniformStruct.SceneTexture!");
	static_assert(offsetof(BrightPassUniformStruct, Threshold) == 8, "Wrong Uniform Buffer offset: BrightPassUniformStruct.Threshold!");

	class BrightPass : public RenderPass
	{

	public:

		BrightPass(Renderer* renderer);
		virtual ~BrightPass();

		void Init();
		void Execute(const Texture2DRef& sceneTexture);

		inline void SetThreshold(const float threshold) { Threshold = threshold; }
		inline float GetThreshold() const { return Threshold; }

	private:

		UniformBufferRef UniformBuffer;
		BrightPassUniformStruct Uniforms;

		float Threshold;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BlurPass ////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct BlurPassUniformStruct
	{

		alignas(8) uint64 Texture;
		alignas(4) float Horizontal;

	};

	static_assert(sizeof(BlurPassUniformStruct) <= 32, "Wrong Uniform Buffer size: BlurPassUniformStruct!");
	static_assert(offsetof(BlurPassUniformStruct, Texture) == 0, "Wrong Uniform Buffer offset: BlurPassUniformStruct.Texture!");
	static_assert(offsetof(BlurPassUniformStruct, Horizontal) == 8, "Wrong Uniform Buffer offset: BlurPassUniformStruct.Horizontal!");

	class BlurPass : public RenderPass
	{

	public:

		BlurPass(Renderer* renderer);
		virtual ~BlurPass();

		void Init();
		void Execute(const Texture2DRef& sourceTexture, const bool horizontal);

	private:

		UniformBufferRef UniformBuffer;
		BlurPassUniformStruct Uniforms;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BlendPass ///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	// These values correspond directly to the shader!
	enum class BlendPassMode
	{

		None		= 0,
		Additive	= 1,
		Screen		= 2

	};

	struct BlendPassUniformStruct
	{

		alignas(4) int32 BlendMode;

		alignas(8) uint64 Texture1;
		alignas(8) uint64 Texture2;

		alignas(4) float UseTexture1BicubicUpscale;
		alignas(4) float UseTexture2BicubicUpscale;

		alignas(4) float Texture1Mix;
		alignas(4) float Texture2Mix;

	};

	static_assert(sizeof(BlendPassUniformStruct) <= 48, "Wrong Uniform Buffer size: BlendPassUniformStruct!");
	static_assert(offsetof(BlendPassUniformStruct, BlendMode) == 0, "Wrong Uniform Buffer offset: BlendPassUniformStruct.BlendMode!");
	static_assert(offsetof(BlendPassUniformStruct, Texture1) == 8, "Wrong Uniform Buffer offset: BlendPassUniformStruct.Texture1!");
	static_assert(offsetof(BlendPassUniformStruct, Texture2) == 16, "Wrong Uniform Buffer offset: BlendPassUniformStruct.Texture2!");
	static_assert(offsetof(BlendPassUniformStruct, UseTexture1BicubicUpscale) == 24, "Wrong Uniform Buffer offset: BlendPassUniformStruct.UseTexture1BicubicUpscale!");
	static_assert(offsetof(BlendPassUniformStruct, UseTexture2BicubicUpscale) == 28, "Wrong Uniform Buffer offset: BlendPassUniformStruct.UseTexture2BicubicUpscale!");
	static_assert(offsetof(BlendPassUniformStruct, Texture1Mix) == 32, "Wrong Uniform Buffer offset: BlendPassUniformStruct.Texture1Mix!");
	static_assert(offsetof(BlendPassUniformStruct, Texture2Mix) == 36, "Wrong Uniform Buffer offset: BlendPassUniformStruct.Texture2Mix!");

	class BlendPass : public RenderPass
	{

	public:

		BlendPass(Renderer* renderer);
		virtual ~BlendPass();

		void Init();
		void Execute(const Texture2DRef& sourceTexture1, const Texture2DRef& sourceTexture2);

	private:

		UniformBufferRef UniformBuffer;
		BlendPassUniformStruct Uniforms;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BloomStage //////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class BloomParameters
	{

	public:

		inline void SetThreshold(const float threshold) { Threshold = threshold; }
		inline void SetBlurPassCount(const uint32 count) { BlurPassCount = count; }

		inline float GetThreshold() const { return Threshold; }
		inline uint32 GetBlurPassCount() const { return BlurPassCount; }

	private:

		float Threshold;
		uint32 BlurPassCount;

	};

	class BloomStage : public RenderStage
	{

	public:

		BloomStage(Renderer* renderer);
		virtual ~BloomStage();

		void Init();
		void Execute(const Texture2DRef& sceneTexture);

		inline const Texture2DRef& GetTargetTexture() const { return TargetTexture; }

	private:

		void ExecuteBrightnessPass(const Texture2DRef& sceneTexture);
		void ExecuteBlurPass(const Texture2DRef& sourceTexture);
		void ExecuteBlendPass(const Texture2DRef& sceneTexture);

	private:

		BrightPass* BrightPass;
		BlurPass* BlurPass;
		BlendPass* BlendPass;

		ShaderRef BrightPassShader;
		ShaderRef BlurPassShader;
		ShaderRef BlendPassShader;

		FrameBufferRef TargetFrameBuffer;
		FrameBufferRef BloomFrameBuffer;
		FrameBufferRef FrameBufferH;
		FrameBufferRef FrameBufferV;

		Texture2DRef TargetTexture;
		Texture2DRef BloomTexture;
		Texture2DRef BlurTextureH;
		Texture2DRef BlurTextureV;

	};

}