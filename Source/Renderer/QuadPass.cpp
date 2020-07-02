#include "pch.h"

#include "PBR/Renderer/Renderer.h"
#include "PBR/Renderer/QuadPass.h"

#include "PBR/ShaderLibrary.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// QuadPass ////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QuadPass::QuadPass(Renderer* renderer)
		: RenderPass(renderer)
	{

	}

	QuadPass::~QuadPass()
	{

	}

	void QuadPass::Init()
	{
		UniformBuffer = UniformBuffer::Create(sizeof(QuadPassUniformStruct), BufferUsage::Dynamic);
	}

	void QuadPass::Execute(const Texture2DRef& texture)
	{
		Shader->Bind();

		UniformBuffer->BindBlock(0);
		Shader->BindBlock("QuadData", 0);


		Uniforms.Texture = texture->GetShaderHandle();
		UniformBuffer->SetData(&Uniforms, sizeof(QuadPassUniformStruct));

		RendererInstance->DrawQuad();


		Shader->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// QuadStage ///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QuadStage::QuadStage(Renderer* renderer)
		: QuadPass(new ::EngineCore::QuadPass(renderer))
	{

	}

	QuadStage::~QuadStage()
	{
		delete QuadPass;
	}

	void QuadStage::Init()
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		QuadShader = shaders->Get("QuadPass");

		QuadPass->Init();
		QuadPass->SetShader(QuadShader);
	}

	void QuadStage::Execute(const Texture2DRef& texture)
	{
		QuadPass->Execute(texture);
	}

}