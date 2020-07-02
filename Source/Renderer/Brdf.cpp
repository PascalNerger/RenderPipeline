#include "pch.h"

#include "PBR/Renderer/Brdf.h"
#include "PBR/Renderer/Renderer.h"

#include "PBR/ShaderLibrary.h"

namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BrdfPass ////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	BrdfPass::BrdfPass(Renderer* renderer)
		: RenderPass(renderer)
	{

	}

	BrdfPass::~BrdfPass()
	{

	}

	void BrdfPass::Init()
	{
	}

	void BrdfPass::Execute()
	{
		Shader->Bind();

		FrameBuffer->Bind();
		FrameBuffer->Attach(RenderTarget);
		FrameBuffer->Clear();

		RendererInstance->DrawQuad();

		FrameBuffer->Unbind();
		Shader->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BrdfStage ///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	BrdfStage::BrdfStage(Renderer* renderer)
		: BrdfPass(new ::EngineCore::BrdfPass(renderer))
	{

	}

	BrdfStage::~BrdfStage()
	{
		delete BrdfPass;
	}

	void BrdfStage::Init()
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		BrdfShader = shaders->Get("BrdfPass");


		BrdfFrameBuffer = FrameBuffer::Create(512, 512);

		TextureCreateInfo brdfTextureCreateInfo;
		brdfTextureCreateInfo.DataType = TextureDataType::Float;
		brdfTextureCreateInfo.Format = TextureFormat::Rg;
		brdfTextureCreateInfo.InternalFormat = TextureFormat::Rg16f;
		brdfTextureCreateInfo.Filter = TextureFilter::Linear;
		brdfTextureCreateInfo.Wrap = TextureWrap::Clamp;
		brdfTextureCreateInfo.UseMipMaps = false;

		BrdfTexture = Texture2D::Create(512, 512, brdfTextureCreateInfo);
		

		BrdfPass->Init();
		BrdfPass->SetShader(BrdfShader);
		BrdfPass->SetFrameBuffer(BrdfFrameBuffer);
		BrdfPass->SetRenderTarget(BrdfTexture);
	}

	void BrdfStage::Execute()
	{
		BrdfPass->Execute();
	}

}