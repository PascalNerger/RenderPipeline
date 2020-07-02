#include "pch.h"

#include "PBR/Renderer/Renderer.h"
#include "PBR/Renderer/ScenePass.h"

#include "PBR/ShaderLibrary.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ScenePass ///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ScenePass::ScenePass(Renderer* renderer)
		: RenderPass(renderer)
		, SceneBuffer()
		, LightBuffer()
		, MaterialBuffer()
		, SceneUniforms()
		, ImageUniforms()
	{
	}

	ScenePass::~ScenePass()
	{

	}

	void ScenePass::Init()
	{
		SceneBuffer = UniformBuffer::Create(sizeof(SceneUniformStruct), BufferUsage::Dynamic);
		LightBuffer = UniformBuffer::Create(sizeof(LightUniformStruct) * 4, BufferUsage::Dynamic);
		MaterialBuffer = UniformBuffer::Create(sizeof(MaterialUniformStruct), BufferUsage::Dynamic);
		ImageBuffer = UniformBuffer::Create(sizeof(ImageUniformStruct), BufferUsage::Dynamic);
		ShadowBuffer = UniformBuffer::Create(sizeof(ShadowUniformStruct), BufferUsage::Dynamic);
	}

	void ScenePass::Execute(const RenderView& view, const TextureCubeRef& irradianceMap, const TextureCubeRef& preFilteredMap, const Texture2DRef& brdfTexture, const Texture2DRef& shadowMap)
	{
		RHICommandListImmediate& commandList = RendererInstance->GetCommandList();
		commandList.SetCullMode(CullMode::Back);

		Shader->Bind();

		SceneBuffer->BindBlock(0);
		Shader->BindBlock("SceneData", 0);

		LightBuffer->BindBlock(1);
		Shader->BindBlock("Lights", 1);

		MaterialBuffer->BindBlock(2);
		Shader->BindBlock("Material", 2);

		ImageBuffer->BindBlock(3);
		Shader->BindBlock("ImageData", 3);

		ShadowBuffer->BindBlock(4);
		Shader->BindBlock("ShadowData", 4);

		FrameBuffer->Bind();
		FrameBuffer->Attach(RenderTarget);
		FrameBuffer->Clear();


		SetImageUniforms(irradianceMap, preFilteredMap, brdfTexture);
		SetShadowUniforms(shadowMap, view.GetSun());

		for (const RenderPrimitive& primitive : view.GetRenderPrimitives())
		{
			const MeshRef mesh = primitive.GetMesh();
			const MaterialRef material = primitive.GetMaterial();
			const Transform& transform = primitive.GetTransform();

			SetSceneUniforms(view, transform, false);
			SetLightUniforms(view, transform.GetPosition());
			SetMaterialUniforms(material);

			RendererInstance->DrawMesh(mesh);
		}

		for (const InstancedPrimitive& primitive : view.GetInstancedPrimitves())
		{
			const InstancedMeshRef mesh = primitive.GetMesh();
			const MaterialRef material = primitive.GetMaterial();

			SetSceneUniforms(view, Transform(), true);
			SetLightUniforms(view, glm::vec3()); // TODO: Instanced object position?
			SetMaterialUniforms(material);

			RendererInstance->DrawInstancedMesh(mesh);
		}

		FrameBuffer->Unbind();
		Shader->Unbind();
	}

	void ScenePass::SetSceneUniforms(const RenderView& view, const Transform& model, const bool instanced)
	{
		SceneUniforms.CameraPosition = view.GetCamera()->GetPosition();
		SceneUniforms.ViewMatrix = view.GetCamera()->GetViewMatrix();
		SceneUniforms.ProjectionMatrix = view.GetCamera()->GetProjectionMatrix();

		if(!instanced)
			SceneUniforms.ModelMatrix = model.GetMatrix();

		SceneUniforms.UseInstancing = instanced ? 1.0f : 0.0f;

		SceneBuffer->SetData(&SceneUniforms, sizeof(SceneUniformStruct));
	}

	void ScenePass::SetLightUniforms(const RenderView& view, const glm::vec3& point)
	{
		struct LightPositionSortFunctor
		{

		public:

			LightPositionSortFunctor(const glm::vec3& point)
				: Point(point)
			{
			}

			bool operator()(const LightRef& lhs, const LightRef& rhs) const
			{
				if (lhs->GetType() == LightType::Directional)
					return true;

				float lhsDistance = glm::distance(lhs->GetTransform().GetPosition(), Point);
				float rhsDistance = glm::distance(rhs->GetTransform().GetPosition(), Point);

				return lhsDistance < rhsDistance;
			}

		private:

			glm::vec3 Point;

		};

		std::vector<LightRef> affectingLights;

		for (const LightRef& light : view.GetLights())
		{
			bool affects = light->Affects(point);
			if (affects)
				affectingLights.push_back(light);
		}

		std::sort(affectingLights.begin(), affectingLights.end(), LightPositionSortFunctor(point));


		uint32 offset = 0;
		LightUniformStruct lightUniforms;

		uint32 maxLights = 4;

		SunRef sun = view.GetSun();
		if (sun)
		{
			sun->GetShaderParameters(lightUniforms);
			LightBuffer->SetData(&lightUniforms, sizeof(LightUniformStruct), offset);

			offset += sizeof(LightUniformStruct);
			maxLights = 3;
		}

		for (uint32 i = 0; i < affectingLights.size(); ++i)
		{
			if (i > maxLights)
				break;

			affectingLights[i]->GetShaderParameters(lightUniforms);
			LightBuffer->SetData(&lightUniforms, sizeof(LightUniformStruct), offset);

			offset += sizeof(LightUniformStruct);
		}
	}

	void ScenePass::SetMaterialUniforms(const MaterialRef& material)
	{
		MaterialUniformStruct materialUniforms;
		material->GetShaderParameters(materialUniforms);

		MaterialBuffer->SetData(&materialUniforms, sizeof(MaterialUniformStruct));
	}

	void ScenePass::SetImageUniforms(const TextureCubeRef& irradianceMap, const TextureCubeRef& preFilteredMap, const Texture2DRef& brdfTexture)
	{
		ImageUniforms.UseImageBasedLighting = irradianceMap ? 1.0f : 0.0f;

		ImageUniforms.IrradianceMap = irradianceMap->GetShaderHandle();
		ImageUniforms.PreFilteredMap = preFilteredMap->GetShaderHandle();
		ImageUniforms.BrdfTexture = brdfTexture->GetShaderHandle();

		ImageBuffer->SetData(&ImageUniforms, sizeof(ImageUniformStruct));
	}

	void ScenePass::SetShadowUniforms(const Texture2DRef& shadowMap, const SunRef& sun)
	{
		ShadowUniforms.UseShadowMap = shadowMap ? 1.0f : 0.0f;
		ShadowUniforms.ShadowBias = 0.005f;
		ShadowUniforms.ShadowMap = shadowMap ? shadowMap->GetShaderHandle() : 0;

		ShadowUniforms.LightViewMatrix = sun->GetView();
		ShadowUniforms.LightProjectionMatrix = sun->GetProjection();

		ShadowBuffer->SetData(&ShadowUniforms, sizeof(ShadowUniformStruct));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// SceneStage //////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	SceneStage::SceneStage(Renderer* renderer)
		: ScenePass(new ::EngineCore::ScenePass(renderer))
	{

	}

	SceneStage::~SceneStage()
	{
		delete ScenePass;
	}

	void SceneStage::Init()
	{
		ShaderLibrary* shaders = ShaderLibrary::GetInstance();
		CommonShader = shaders->Get("CommonPass");

		CommonFrameBuffer = FrameBuffer::Create(1280, 720);

		TextureCreateInfo createInfo;
		createInfo.DataType = TextureDataType::Float;
		createInfo.Format = TextureFormat::Rgba;
		createInfo.InternalFormat = TextureFormat::Rgba16f;
		createInfo.Filter = TextureFilter::Linear;
		createInfo.Wrap = TextureWrap::Clamp;
		createInfo.UseMipMaps = false;

		TargetTexture = Texture2D::Create(1280, 720, createInfo);

		ScenePass->Init();
		ScenePass->SetShader(CommonShader);
		ScenePass->SetFrameBuffer(CommonFrameBuffer);
		ScenePass->SetRenderTarget(TargetTexture);
	}

	void SceneStage::SceneStage::Execute(const RenderView& view, const TextureCubeRef& irradianceMap, const TextureCubeRef& preFilteredMap, const Texture2DRef& brdfTexture, const Texture2DRef& shadowMap)
	{
		ScenePass->Execute(view, irradianceMap, preFilteredMap, brdfTexture, shadowMap);
	}

}