#include "pch.h"
#include "PBR/Renderer/EnvPreFilterPass.h"

#include <glm/gtc/matrix_transform.hpp>

#include "PBR/Renderer/Renderer.h"
#include "PBR/ShaderLibrary.h"

namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// EnvPreFilterPass ////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	EnvPreFilterPass::EnvPreFilterPass(Renderer* renderer)
		: RenderPass(renderer)
	{

	}

	EnvPreFilterPass::~EnvPreFilterPass()
	{

	}

	void EnvPreFilterPass::Init()
	{
		UniformBuffer = UniformBuffer::Create(sizeof(EnvPreFilterUniformStruct), BufferUsage::Dynamic);

		CaptureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		CaptureViewMatrices[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		CaptureViewMatrices[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		CaptureViewMatrices[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		CaptureViewMatrices[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		CaptureViewMatrices[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		CaptureViewMatrices[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	}

	void EnvPreFilterPass::Execute(const TextureCubeRef& environmentMap)
	{
		FrameBuffer->Bind();
		Shader->Bind();

		UniformBuffer->BindBlock(0);
		Shader->BindBlock("PreFilterInfo", 0);

		Uniforms.ProjectionMatrix = CaptureProjection;
		Uniforms.EnvironmentMap = environmentMap->GetShaderHandle();

		RHICommandListImmediate& commandListImmediate = RendererInstance->GetCommandList();
		commandListImmediate.SetCullMode(CullMode::None);

		const uint32 maxMipLevels = 5;
		for (uint32 mip = 0; mip < maxMipLevels; ++mip)
		{

			uint32 mipWidth = static_cast<uint32>(1024 * std::pow(0.5f, mip));
			uint32 mipHeight = static_cast<uint32>(1024 * std::pow(0.5f, mip));

			FrameBuffer->Resize(mipWidth, mipHeight);

			const float roughness = static_cast<float>(mip) / (static_cast<float>(maxMipLevels) - 1.0f);

			Uniforms.Roughness = roughness;
			Uniforms.MapResolution = static_cast<float>(mipWidth);

			for (uint32 i = 0; i < 6; ++i)
			{
				Uniforms.ViewMatrix = CaptureViewMatrices[i];

				FrameBuffer->Attach(CubeRenderTarget, CubeMapOrientation(uint32(CubeMapOrientation::PositiveX) + i), TextureAttachment::Color, mip);
				FrameBuffer->Clear();

				UniformBuffer->SetData(&Uniforms, sizeof(EnvPreFilterUniformStruct));
				RendererInstance->DrawCube();
			}
		}

		commandListImmediate.SetCullMode(CullMode::Back);

		FrameBuffer->Unbind();
		Shader->Unbind();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// EnvPreFilterStage ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	EnvPreFilterStage::EnvPreFilterStage(Renderer* renderer)
		: PreFilterPass(new EnvPreFilterPass(renderer))
	{

	}

	EnvPreFilterStage::~EnvPreFilterStage()
	{
		delete PreFilterPass;
	}

	void EnvPreFilterStage::Init()
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		PreFilterShader = shaders->Get("EnvPreFilterPass");

		PreFilterFrameBuffer = FrameBuffer::Create(1024, 1024);

		TextureCreateInfo createInfo;
		createInfo.DataType = TextureDataType::Float;
		createInfo.Format = TextureFormat::Rgba;
		createInfo.InternalFormat = TextureFormat::Rgba16f;
		createInfo.Wrap = TextureWrap::Clamp;
		createInfo.Filter = TextureFilter::Linear;
		createInfo.UseMipMaps = true;

		PreFilterCube = TextureCube::Create(1024, 1024, createInfo);


		PreFilterPass->Init();
		PreFilterPass->SetShader(PreFilterShader);
		PreFilterPass->SetFrameBuffer(PreFilterFrameBuffer);
		PreFilterPass->SetCubeRenderTarget(PreFilterCube);
	}

	void EnvPreFilterStage::Execute(const TextureCubeRef& environmentMap)
	{
		PreFilterPass->Execute(environmentMap);
	}

}