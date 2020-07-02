#include "pch.h"

#include "PBR/Renderer/Renderer.h"
#include "PBR/Renderer/ShadowPass.h"

#include "PBR/ShaderLibrary.h"

namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ShadowMapPass ///////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ShadowPass::ShadowPass(Renderer* renderer)
		: RenderPass(renderer)
	{

	}

	ShadowPass::~ShadowPass()
	{

	}


	void ShadowPass::Init()
	{
		UniformBuffer = UniformBuffer::Create(sizeof(ShadowPassUniformStruct), BufferUsage::Dynamic);
	}

	void ShadowPass::Execute(const RenderView& view)
	{
		Shader->Bind();
		UniformBuffer->BindBlock(0);
		Shader->BindBlock("ShadowData", 0);

		FrameBuffer->Bind();
		FrameBuffer->Attach(DepthTarget, TextureAttachment::Depth);
		FrameBuffer->Clear();

		RHICommandListImmediate& commandList = RendererInstance->GetCommandList();
		commandList.SetCullMode(CullMode::Front);

		Uniforms.UseInstancing = 0.0f;
		for (const RenderPrimitive& primitive : view.GetRenderPrimitives())
		{
			const MeshRef mesh = primitive.GetMesh();
			const Transform& transform = primitive.GetTransform();

			Uniforms.ModelMatrix = transform.GetMatrix();
			Uniforms.LightViewMatrix = view.GetSun()->GetView();
			Uniforms.LightProjectionMatrix = view.GetSun()->GetProjection();

			UniformBuffer->SetData(&Uniforms, sizeof(ShadowPassUniformStruct));
			RendererInstance->DrawMesh(mesh);
		}

		Uniforms.UseInstancing = 1.0f;
		for (const InstancedPrimitive& primitive : view.GetInstancedPrimitves())
		{
			const InstancedMeshRef mesh = primitive.GetMesh();
			const MaterialRef material = primitive.GetMaterial();

			Uniforms.LightViewMatrix = view.GetSun()->GetView();
			Uniforms.LightProjectionMatrix = view.GetSun()->GetProjection();

			UniformBuffer->SetData(&Uniforms, sizeof(ShadowPassUniformStruct));
			RendererInstance->DrawInstancedMesh(mesh);
		}

		commandList.SetCullMode(CullMode::Back);

		FrameBuffer->Unbind();
		Shader->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ShadowStage /////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ShadowStage::ShadowStage(Renderer* renderer)
		: ShadowPass(new ::EngineCore::ShadowPass(renderer))
	{

	}

	ShadowStage::~ShadowStage()
	{
		delete ShadowPass;
	}

	void ShadowStage::Init()
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		ShadowShader = shaders->Get("ShadowPass");

		ShadowFrameBuffer = FrameBuffer::Create(2048, 2048, { false });

		TextureCreateInfo createInfo;
		createInfo.Format = TextureFormat::Depth;
		createInfo.InternalFormat = TextureFormat::Depth32;
		createInfo.Filter = TextureFilter::Linear;
		createInfo.Wrap = TextureWrap::Clamp;
		createInfo.UseMipMaps = false;
		createInfo.IsShadowTexture = true;

		DepthTexture = Texture2D::Create(2048, 2048, createInfo);
		

		ShadowPass->Init();
		ShadowPass->SetShader(ShadowShader);
		ShadowPass->SetFrameBuffer(ShadowFrameBuffer);
		ShadowPass->SetDepthTarget(DepthTexture);
	}

	void ShadowStage::Execute(const RenderView& renderView)
	{
		ShadowPass->Execute(renderView);
	}

}