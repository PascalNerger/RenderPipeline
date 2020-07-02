#include "pch.h"

#include "PBR/Renderer/Renderer.h"
#include "PBR/Renderer/Fxaa.h"

#include "PBR/ShaderLibrary.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// FxaaPass ////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	FxaaPass::FxaaPass(Renderer* renderer)
		: RenderPass(renderer)
	{

	}

	FxaaPass::~FxaaPass()
	{

	}

	void FxaaPass::Init()
	{
		UniformBuffer = UniformBuffer::Create(sizeof(FxaaPassUniformStruct), BufferUsage::Dynamic);
		Uniforms.DisplayEdges = 0.0f;
	}

	void FxaaPass::Execute(const Texture2DRef& source)
	{
		Shader->Bind();

		UniformBuffer->BindBlock(0);
		Shader->BindBlock("FxaaData", 0);

		FrameBuffer->Bind();
		FrameBuffer->Attach(RenderTarget);
		FrameBuffer->Clear();

		Uniforms.SourceTexture = source->GetShaderHandle();
		UniformBuffer->SetData(&Uniforms, sizeof(FxaaPassUniformStruct));

		RendererInstance->DrawQuad();

		FrameBuffer->Unbind();
		Shader->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// FxaaStage ///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	FxaaStage::FxaaStage(Renderer* renderer)
		: FxaaPass(new ::EngineCore::FxaaPass(renderer))
	{

	}

	FxaaStage::~FxaaStage()
	{
		delete FxaaPass;
	}

	void FxaaStage::Init()
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		FxaaShader = shaders->Get("Fxaa");

		FxaaFrameBuffer = FrameBuffer::Create(1280, 720);

		TextureCreateInfo createInfo;
		createInfo.DataType = TextureDataType::Float;
		createInfo.Format = TextureFormat::Rgba;
		createInfo.InternalFormat = TextureFormat::Rgba16f;
		createInfo.Filter = TextureFilter::Linear;
		createInfo.Wrap = TextureWrap::Clamp;
		createInfo.UseMipMaps = false;

		FxaaTarget = Texture2D::Create(1280, 720, createInfo);

		FxaaPass->Init();
		FxaaPass->SetShader(FxaaShader);
		FxaaPass->SetFrameBuffer(FxaaFrameBuffer);
		FxaaPass->SetRenderTarget(FxaaTarget);
	}

	void FxaaStage::Execute(const Texture2DRef& source)
	{
		FxaaPass->Execute(source);
	}

}