#include "pch.h"

#include <glm/gtc/matrix_transform.hpp>

#include "PBR/Renderer/Renderer.h"
#include "PBR/Renderer/SphereMapPass.h"

#include "PBR/ShaderLibrary.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SphereMapPass ///////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	SphereMapPass::SphereMapPass(Renderer* renderer)
		: RenderPass(renderer)
	{

	}

	SphereMapPass::~SphereMapPass()
	{

	}

	void SphereMapPass::Init()
	{
		UniformBuffer = UniformBuffer::Create(sizeof(SphereMapPassUniformStruct), BufferUsage::Dynamic);

		CaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		CaptureViewMatrices[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		CaptureViewMatrices[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		CaptureViewMatrices[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CaptureViewMatrices[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		CaptureViewMatrices[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		CaptureViewMatrices[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	}

	void SphereMapPass::Execute(const Texture2DRef& sphereMap)
	{
		Shader->Bind();
		UniformBuffer->BindBlock(0);
		Shader->BindBlock("SphereMapData", 0);

		FrameBuffer->Bind();

		Uniforms.ProjectionMatrix = CaptureProjection;
		Uniforms.SphereMap = sphereMap->GetShaderHandle();

		RHICommandListImmediate& commandList = RendererInstance->GetCommandList();
		commandList.SetCullMode(CullMode::None);

		for (uint32 i = 0; i < 6; ++i)
		{
			Uniforms.ViewMatrix = CaptureViewMatrices[i];
			UniformBuffer->SetData(&Uniforms, sizeof(SphereMapPassUniformStruct));

			FrameBuffer->Attach(CubeRenderTarget, CubeMapOrientation(uint32(CubeMapOrientation::PositiveX) + i));
			FrameBuffer->Clear();

			RendererInstance->DrawCube();
		}

		commandList.SetCullMode(CullMode::Back);


		FrameBuffer->Unbind();
		Shader->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SphereMapStage //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	SphereMapStage::SphereMapStage(Renderer* renderer)
		: SpherePass(new SphereMapPass(renderer))
	{

	}

	SphereMapStage::~SphereMapStage()
	{
		delete SpherePass;
	}

	void SphereMapStage::Init()
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		SphereShader = shaders->Get("SphereMapPass");

		const uint32 width = 2048;
		const uint32 height = 2048;

		SphereFrameBuffer = FrameBuffer::Create(width, height);

		TextureCreateInfo createInfo;
		createInfo.DataType = TextureDataType::Float;
		createInfo.Format = TextureFormat::Rgba;
		createInfo.InternalFormat = TextureFormat::Rgba16f;
		createInfo.Filter = TextureFilter::Linear;
		createInfo.Wrap = TextureWrap::Clamp;
		createInfo.UseMipMaps = false;
		
		CubeMap = TextureCube::Create(width, height, createInfo);


		SpherePass->Init();
		SpherePass->SetShader(SphereShader);
		SpherePass->SetFrameBuffer(SphereFrameBuffer);
		SpherePass->SetCubeRenderTarget(CubeMap);
	}

	void SphereMapStage::Execute(const Texture2DRef& sphereMap)
	{
		SpherePass->Execute(sphereMap);
	}

}