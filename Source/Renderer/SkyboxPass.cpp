#include "pch.h"

#include "PBR/Renderer/Renderer.h"
#include "PBR/Renderer/SkyboxPass.h"

#include "PBR/ShaderLibrary.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SkyboxPass //////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	SkyboxPass::SkyboxPass(Renderer* renderer)
		: RenderPass(renderer)
	{

	}

	SkyboxPass::~SkyboxPass()
	{

	}

	void SkyboxPass::Init()
	{
		UniformBuffer = UniformBuffer::Create(sizeof(SkyboxPassUniformStruct), BufferUsage::Dynamic);
	}

	void SkyboxPass::Execute(const CameraRef& camera, const TextureCubeRef& skybox)
	{
		RHICommandListImmediate& commandList = RendererInstance->GetCommandList();

		Shader->Bind();
		UniformBuffer->BindBlock(0);
		Shader->BindBlock("SkyboxInfo", 0);

		FrameBuffer->Bind();
		FrameBuffer->Attach(RenderTarget);

		// Do not clear, because this is the scene stage framebuffer
		// FrameBuffer->Clear();

		// Remove translation from view matrix
		glm::mat4 viewMatrix = camera->GetViewMatrix();
		viewMatrix[3][0] = 0;
		viewMatrix[3][1] = 0;
		viewMatrix[3][2] = 0;

		Uniforms.ViewMatrix = viewMatrix;
		Uniforms.ProjectionMatrix = camera->GetProjectionMatrix();
		Uniforms.Skybox = skybox->GetShaderHandle();


		commandList.DepthMask(false);
		commandList.SetCullMode(CullMode::None);
		commandList.SetDepthFunc(DepthFunc::LessEqual);

		UniformBuffer->SetData(&Uniforms, sizeof(SkyboxPassUniformStruct));
		RendererInstance->DrawCube();

		commandList.SetDepthFunc(DepthFunc::Less);
		commandList.SetCullMode(CullMode::Back);
		commandList.DepthMask(true);


		FrameBuffer->Unbind();
		Shader->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SkyboxStage /////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	SkyboxStage::SkyboxStage(Renderer* renderer)
		: SkyboxPass(new ::EngineCore::SkyboxPass(renderer))
	{

	}

	SkyboxStage::~SkyboxStage()
	{
		delete SkyboxPass;
	}

	void SkyboxStage::Init(const SceneStage* sceneStage)
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		SkyboxShader = shaders->Get("SkyboxPass");

		// Use framebuffer and target texture from scene stage
		SkyboxFrameBuffer = sceneStage->CommonFrameBuffer;
		TargetTexture = sceneStage->TargetTexture;

		SkyboxPass->Init();
		SkyboxPass->SetShader(SkyboxShader);
		SkyboxPass->SetFrameBuffer(SkyboxFrameBuffer);
		SkyboxPass->SetRenderTarget(TargetTexture);
	}

	void SkyboxStage::Execute(const CameraRef& camera, const TextureCubeRef& skybox)
	{
		SkyboxPass->Execute(camera, skybox);
	}


}