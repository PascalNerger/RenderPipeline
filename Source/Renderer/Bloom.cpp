#include "pch.h"

#include "PBR/Renderer/Bloom.h"
#include "PBR/RHI/CommandList.h"
#include "PBR/Renderer/Renderer.h"

#include "PBR/ShaderLibrary.h"

namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BrightnessPass //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	BrightPass::BrightPass(Renderer* renderer)
		: RenderPass(renderer)
		, UniformBuffer()
		, Uniforms()
		, Threshold()
	{
		std::memset(&Uniforms, 0, sizeof(BrightPassUniformStruct));
	}

	BrightPass::~BrightPass()
	{

	}

	void BrightPass::Init()
	{
		UniformBuffer = UniformBuffer::Create(sizeof(BrightPassUniformStruct), BufferUsage::Dynamic);
		Threshold = 1.0f;
	}

	void BrightPass::Execute(const Texture2DRef& sceneTexture)
	{
		Shader->Bind();

		UniformBuffer->BindBlock(0);
		Shader->BindBlock("BrightPassData", 0);


		FrameBuffer->Bind();
		FrameBuffer->Attach(RenderTarget);

		FrameBuffer->Clear();

		Uniforms.SceneTexture = sceneTexture->GetShaderHandle();
		Uniforms.Threshold = Threshold;
		UniformBuffer->SetData(&Uniforms, sizeof(BrightPassUniformStruct));

		RendererInstance->DrawQuad();

		FrameBuffer->Unbind();
		Shader->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BlurPass ////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	BlurPass::BlurPass(Renderer* renderer)
		: RenderPass(renderer)
		, UniformBuffer()
		, Uniforms()
	{
		std::memset(&Uniforms, 0, sizeof(BlurPassUniformStruct));
	}

	BlurPass::~BlurPass()
	{

	}

	void BlurPass::Init()
	{
		UniformBuffer = UniformBuffer::Create(sizeof(BlurPassUniformStruct), BufferUsage::Dynamic);
	}

	void BlurPass::Execute(const Texture2DRef& sourceTexture, const bool horizontal)
	{
		Shader->Bind();

		UniformBuffer->BindBlock(0);
		Shader->BindBlock("BlurInfo", 0);

		FrameBuffer->Bind();
		FrameBuffer->Attach(RenderTarget);
		FrameBuffer->Clear();

		Uniforms.Texture = sourceTexture->GetShaderHandle();
		Uniforms.Horizontal = horizontal ? 1.0f : 0.0f;

		UniformBuffer->SetData(&Uniforms, sizeof(BlurPassUniformStruct));
		RendererInstance->DrawQuad();

		FrameBuffer->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BlendPass ///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	BlendPass::BlendPass(Renderer* renderer)
		: RenderPass(renderer)
		, UniformBuffer()
		, Uniforms()
	{
		std::memset(&Uniforms, 0, sizeof(BlendPassUniformStruct));
	}

	BlendPass::~BlendPass()
	{

	}

	void BlendPass::Init()
	{
		UniformBuffer = UniformBuffer::Create(sizeof(BlendPassUniformStruct), BufferUsage::Dynamic);

		Uniforms.BlendMode = int32(BlendPassMode::Additive);

		Uniforms.UseTexture1BicubicUpscale = 0.0f;
		Uniforms.UseTexture2BicubicUpscale = 1.0f;

		Uniforms.Texture1Mix = 1.0f;
		Uniforms.Texture2Mix = 0.75f;
	}

	void BlendPass::Execute(const Texture2DRef& sourceTexture1, const Texture2DRef& sourceTexture2)
	{
		Shader->Bind();
		UniformBuffer->BindBlock(0);
		Shader->BindBlock("Textures", 0);

		FrameBuffer->Bind();
		FrameBuffer->Attach(RenderTarget);
		FrameBuffer->Clear();

		Uniforms.Texture1 = sourceTexture1->GetShaderHandle();
		Uniforms.Texture2 = sourceTexture2->GetShaderHandle();

		UniformBuffer->SetData(&Uniforms, sizeof(BlendPassUniformStruct));
		RendererInstance->DrawQuad();

		Shader->Unbind();
		FrameBuffer->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BloomStage //////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	BloomStage::BloomStage(Renderer* renderer)
		: BrightPass(new ::EngineCore::BrightPass(renderer))
		, BlurPass(new ::EngineCore::BlurPass(renderer))
		, BlendPass(new ::EngineCore::BlendPass(renderer))
	{
	}

	BloomStage::~BloomStage()
	{
		delete BrightPass;
		delete BlurPass;
		delete BlendPass;
	}

	void BloomStage::Init()
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		BrightPassShader = shaders->Get("BrightPass");
		BlurPassShader = shaders->Get("BlurPass");
		BlendPassShader = shaders->Get("BlendPass");

		BrightPass->Init();
		BrightPass->SetShader(BrightPassShader);

		BlurPass->Init();
		BlurPass->SetShader(BlurPassShader);

		BlendPass->Init();
		BlendPass->SetShader(BlendPassShader);

		// TODO:
		const uint32 width = static_cast<uint32>(1280.0f * (1.0f / 2.0f));
		const uint32 height = static_cast<uint32>(720.0f * (1.0f / 2.0f));

		TargetFrameBuffer = FrameBuffer::Create(1280, 720);
		BloomFrameBuffer = FrameBuffer::Create(width, height);
		FrameBufferH = FrameBuffer::Create(width, height);
		FrameBufferV = FrameBuffer::Create(width, height);

		TextureCreateInfo createInfo;
		createInfo.DataType = TextureDataType::Float;
		createInfo.Format = TextureFormat::Rgba;
		createInfo.InternalFormat = TextureFormat::Rgba16f;
		createInfo.Filter = TextureFilter::Linear;
		createInfo.Wrap = TextureWrap::Clamp;
		createInfo.UseMipMaps = false;

		TextureCreateInfo bloomTextureCreateInfo = createInfo;
		bloomTextureCreateInfo.UseMipMaps = true;

		TargetTexture = Texture2D::Create(1280, 720, createInfo);
		BloomTexture = Texture2D::Create(width, height, bloomTextureCreateInfo);
		BlurTextureH = Texture2D::Create(width, height, createInfo);
		BlurTextureV = Texture2D::Create(width, height, createInfo);
	}

	void BloomStage::Execute(const Texture2DRef& sceneTexture)
	{
		ExecuteBrightnessPass(sceneTexture);
		ExecuteBlurPass(BloomTexture);
		ExecuteBlendPass(sceneTexture);
	}

	void BloomStage::ExecuteBrightnessPass(const Texture2DRef& sceneTexture)
	{
		BrightPass->SetFrameBuffer(BloomFrameBuffer);
		BrightPass->SetRenderTarget(BloomTexture);

		BrightPass->Execute(sceneTexture);
	}

	void BloomStage::ExecuteBlurPass(const Texture2DRef& sourceTexture)
	{
		// 2 horizontal and 2 vertical passes as done by Crytek
		// Pass 1 H
		{
			BlurPass->SetFrameBuffer(FrameBufferH);
			BlurPass->SetRenderTarget(BlurTextureH);

			BlurPass->Execute(sourceTexture, true);
		}

		// Pass 1 V
		{
			BlurPass->SetFrameBuffer(FrameBufferV);
			BlurPass->SetRenderTarget(BlurTextureV);

			BlurPass->Execute(BlurTextureH, false);
		}

		// Pass 2 H
		{
			BlurPass->SetFrameBuffer(FrameBufferH);
			BlurPass->SetRenderTarget(BlurTextureH);

			BlurPass->Execute(BlurTextureV, true);
		}

		// Pass 2 V
		{
			BlurPass->SetFrameBuffer(FrameBufferV);
			BlurPass->SetRenderTarget(BlurTextureV);

			BlurPass->Execute(BlurTextureH, false);
		}

	}

	void BloomStage::ExecuteBlendPass(const Texture2DRef& sceneTexture)
	{
		BlendPass->SetFrameBuffer(TargetFrameBuffer);
		BlendPass->SetRenderTarget(TargetTexture);

		BlendPass->Execute(sceneTexture, BlurTextureV);
	}

}