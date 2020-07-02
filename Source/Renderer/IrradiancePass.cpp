#include "pch.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PBR/RHI/CommandList.h"
#include "PBR/Renderer/Renderer.h"
#include "PBR/Renderer/IrradiancePass.h"

#include "PBR/ShaderLibrary.h"


namespace EngineCore
{

	IrradiancePass::IrradiancePass(Renderer* renderer)
		: RenderPass(renderer)
	{

	}

	IrradiancePass::~IrradiancePass()
	{

	}

	void IrradiancePass::Init()
	{
		CaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		CaptureViewMatrices[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		CaptureViewMatrices[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		CaptureViewMatrices[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CaptureViewMatrices[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		CaptureViewMatrices[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		CaptureViewMatrices[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

		UniformBuffer = UniformBuffer::Create(sizeof(IrradiancePassUniformStruct), BufferUsage::Dynamic);
	}

	void IrradiancePass::Execute(const TextureCubeRef& cubeMap)
	{
		RHICommandListImmediate& commandList = RendererInstance->GetCommandList();

		Shader->Bind();
		FrameBuffer->Bind();

		UniformBuffer->BindBlock(0);
		Shader->BindBlock("IrradianceInfo", 0);

		Uniforms.ProjectionMatrix = CaptureProjection;
		Uniforms.CubeMap = cubeMap->GetShaderHandle();

		commandList.SetCullMode(CullMode::None);

		for (uint32 i = 0; i < 6; ++i)
		{
			Uniforms.ViewMatrix = CaptureViewMatrices[i];
			UniformBuffer->SetData(&Uniforms, sizeof(IrradiancePassUniformStruct));

			FrameBuffer->Attach(CubeRenderTarget, static_cast<CubeMapOrientation>(static_cast<uint32>(CubeMapOrientation::PositiveX) + i));
			FrameBuffer->Clear();

			RendererInstance->DrawCube();
		}

		commandList.SetCullMode(CullMode::Back);

		FrameBuffer->Unbind();
		Shader->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// IrradianceStage /////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////


	IrradianceStage::IrradianceStage(Renderer* renderer)
		: IrradiancePass(new ::EngineCore::IrradiancePass(renderer))
	{

	}

	IrradianceStage::~IrradianceStage()
	{
		delete IrradiancePass;
	}

	void IrradianceStage::Init()
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		IrradianceShader = shaders->Get("IrradiancePass");


		IrradianceFrameBuffer = FrameBuffer::Create(256, 256);

		TextureCreateInfo createInfo;
		createInfo.DataType = TextureDataType::Float;
		createInfo.Format = TextureFormat::Rgba;
		createInfo.InternalFormat = TextureFormat::Rgba16f;
		createInfo.Filter = TextureFilter::Linear;
		createInfo.Wrap = TextureWrap::Clamp;
		createInfo.UseMipMaps = false;

		IrradianceMap = TextureCube::Create(256, 256, createInfo);


		IrradiancePass->Init();
		IrradiancePass->SetShader(IrradianceShader);
		IrradiancePass->SetFrameBuffer(IrradianceFrameBuffer);
		IrradiancePass->SetCubeRenderTarget(IrradianceMap);
	}

	void IrradianceStage::Execute(const TextureCubeRef& environmentMap)
	{
		IrradiancePass->Execute(environmentMap);
	}


}