#include "pch.h"

#include "PBR/Renderer/Renderer.h"
#include "PBR/Renderer/ToneMapper.h"

#include "PBR/ShaderLibrary.h"

namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ToneMapperPass //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ToneMapperPass::ToneMapperPass(Renderer* renderer)
		: RenderPass(renderer)
	{

	}

	ToneMapperPass::~ToneMapperPass()
	{

	}

	void ToneMapperPass::Init()
	{
		UniformBuffer = UniformBuffer::Create(sizeof(ToneMapperPassUniformStruct), BufferUsage::Dynamic);

		Uniforms.Type = int32(ToneMapper::GodotFilmic);
		Uniforms.Exposure = 1.0f;
	}

	void ToneMapperPass::Execute(const Texture2DRef sourceTexture)
	{
		Shader->Bind();
		UniformBuffer->BindBlock(0);
		Shader->BindBlock("ToneMapperData", 0);

		FrameBuffer->Bind();
		FrameBuffer->Attach(RenderTarget);
		FrameBuffer->Clear();

		Uniforms.HdrSourceTexture = sourceTexture->GetShaderHandle();
		UniformBuffer->SetData(&Uniforms, sizeof(ToneMapperPassUniformStruct));

		RendererInstance->DrawQuad();

		FrameBuffer->Unbind();
		Shader->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ToneMapperStage /////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ToneMapperStage::ToneMapperStage(Renderer* renderer)
		: ToneMapperPass(new ::EngineCore::ToneMapperPass(renderer))
	{

	}

	ToneMapperStage::~ToneMapperStage()
	{
		delete ToneMapperPass;
	}

	void ToneMapperStage::Init()
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		ToneMapperShader = shaders->Get("ToneMapper");

		ToneMapperFrameBuffer = FrameBuffer::Create(1280, 720);

		TextureCreateInfo createInfo;
		createInfo.DataType = TextureDataType::Float;
		createInfo.Format = TextureFormat::Rgba;
		createInfo.InternalFormat = TextureFormat::Rgba16f;
		createInfo.Filter = TextureFilter::Linear;
		createInfo.Wrap = TextureWrap::Clamp;
		createInfo.UseMipMaps = false;

		Texture = Texture2D::Create(1280, 720, createInfo);

		ToneMapperPass->Init();
		ToneMapperPass->SetShader(ToneMapperShader);
		ToneMapperPass->SetFrameBuffer(ToneMapperFrameBuffer);
		ToneMapperPass->SetRenderTarget(Texture);
	}

	void ToneMapperStage::Execute(const Texture2DRef& sourceTexture)
	{
		ToneMapperPass->Execute(sourceTexture);
	}


}