#include "pch.h"

#include "PBR/Renderer/Renderer.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Renderer ////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Renderer::Renderer()
		: CommandList(RHICommandListExecutor::GetImmediateCommandList())
		, Queue()
		, Lights()
		, InstancedPrimitives()
		, Sun()
		, ShadowStage(new ::EngineCore::ShadowStage(this))
		, SceneStage(new ::EngineCore::SceneStage(this))
		, SkyboxStage(new ::EngineCore::SkyboxStage(this))
		, BloomStage(new ::EngineCore::BloomStage(this))
		, ToneMapper(new ::EngineCore::ToneMapperStage(this))
		, FxaaStage(new ::EngineCore::FxaaStage(this))
		, QuadStage(new ::EngineCore::QuadStage(this))
		, IrradianceStage(new ::EngineCore::IrradianceStage(this))
		, PreFilterStage(new ::EngineCore::EnvPreFilterStage(this))
		, BrdfStage(new ::EngineCore::BrdfStage(this))
		, SphereMapStage(new ::EngineCore::SphereMapStage(this))
		, Camera()
		, QuadMesh(CreateQuadMesh())
		, CubeMesh(CreateCubeMesh())
		, bIsShadowsEnabled(true)
		, bIsSkyboxEnabled(true)
		, bIsBloomEnabled(true)
		, bIsToneMappingEnabled(true)
		, bIsFxaaEnabled(true)
	{

	}

	Renderer::~Renderer()
	{
		delete ShadowStage;
		delete SceneStage;
		delete SkyboxStage;
		delete BloomStage;
		delete ToneMapper;
		delete FxaaStage;
		delete QuadStage;

		delete IrradianceStage;
		delete PreFilterStage;
		delete BrdfStage;

		delete SphereMapStage;
	}

	void Renderer::Init()
	{
		ShadowStage->Init();
		SceneStage->Init();
		SkyboxStage->Init(SceneStage);
		BloomStage->Init();
		ToneMapper->Init();
		FxaaStage->Init();
		QuadStage->Init();

		IrradianceStage->Init();
		PreFilterStage->Init();
		BrdfStage->Init();

		SphereMapStage->Init();
	}

	void Renderer::DrawQuad()
	{
		QuadMesh->Renderer_Bind();
		CommandList.DrawIndexed(QuadMesh->GetVertexArray());
	}

	void Renderer::DrawCube()
	{
		CubeMesh->Renderer_Bind();
		CommandList.DrawIndexed(CubeMesh->GetVertexArray());
	}

	void Renderer::DrawMesh(const MeshRef& mesh)
	{
		mesh->Renderer_Bind();
		CommandList.DrawIndexed(mesh->GetVertexArray());
	}

	void Renderer::DrawInstancedMesh(const InstancedMeshRef& mesh)
	{
		mesh->Renderer_Bind();
		CommandList.DrawIndexedInstanced(mesh->GetVertexArray(), mesh->GetInstanceBuffer().GetInstanceCount());
	}

	void Renderer::Submit(const RenderPrimitive& primitive)
	{
		Queue.Submit(primitive);
	}

	void Renderer::Submit(const InstancedPrimitive& primitive)
	{
		InstancedPrimitives.push_back(primitive);
	}

	void Renderer::AddLight(const LightRef& light)
	{
		Lights.push_back(light);
	}

	void Renderer::SetSun(const SunRef& sun)
	{
		Sun = sun;
	}

	void Renderer::DrawScene()
	{
		RenderView renderView = CreateRenderView(Queue);
		Texture2DRef target = nullptr;

		if (bIsShadowsEnabled)
		{
			ShadowStage->Execute(renderView);
		}

		SceneStage->Execute(renderView, IrradianceStage->GetIrradianceMap(), PreFilterStage->GetPreFilteredMap(), BrdfStage->GetBrdfTexture(), bIsShadowsEnabled ? ShadowStage->GetDepthTexture() : nullptr);
		target = SceneStage->GetSceneTexture();

		if (Skybox && bIsSkyboxEnabled)
		{
			SkyboxStage->Execute(Camera, Skybox);
			target = SkyboxStage->GetTargetTexture();
		}

		if (bIsBloomEnabled)
		{
			BloomStage->Execute(target);
			target = BloomStage->GetTargetTexture();
		}

		if (bIsToneMappingEnabled)
		{
			ToneMapper->Execute(target);
			target = ToneMapper->GetToneMappedTexture();
		}

		if (bIsFxaaEnabled)
		{
			FxaaStage->Execute(target);
			target = FxaaStage->GetTargetTexture();
		}

		QuadStage->Execute(target);

		Queue.Clear();
		InstancedPrimitives.clear();
	}

	void Renderer::DrawTexture(const Texture2DRef& texture)
	{
		ToneMapper->Execute(texture);
	}

	void Renderer::PreComputeIndirectLighting(const TextureCubeRef& environmentMap)
	{
		IrradianceStage->Execute(environmentMap);
		PreFilterStage->Execute(environmentMap);

		// TODO: This has to be done only once, not for every environment map
		BrdfStage->Execute();
	}

	const TextureCubeRef& Renderer::DrawSphereMapToCubeMap(const Texture2DRef& sphereMap)
	{
		SphereMapStage->Execute(sphereMap);
		return SphereMapStage->GetCubeMap();
	}

	void Renderer::Clear()
	{
		CommandList.SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		CommandList.Clear(ClearFlags::ColorBuffer | ClearFlags::DepthBuffer);
	}

	RenderView Renderer::CreateRenderView(const RenderQueue& queue)
	{
		RenderView renderView = RenderView();
		renderView.SetCamera(Camera);
		renderView.SetSun(Sun);

		for (const LightRef& light : Lights)
			renderView.AddLight(light);

		Queue.OrderByPrimitiveCount();

		for (RenderPrimitiveGroup& group : Queue.GetPrimitiveGroups())
		{
			group.OrderByDistance(Camera->GetPosition());

			for (const RenderPrimitive& primitive : group)
				renderView.AddPrimitive(primitive);
		}

		for (const InstancedPrimitive& primitive : InstancedPrimitives)
			renderView.AddInstancedPrimitive(primitive);

		return renderView;
	}

	MeshRef Renderer::CreateQuadMesh() const
	{
		const float data[] = {
			-1.0f, -1.0f, 0.0f, 0.0f, // bottom left
			 1.0f, -1.0f, 1.0f, 0.0f, // bottom right
			 1.0f,  1.0f, 1.0f, 1.0f, // top right
			-1.0f,  1.0f, 0.0f, 1.0f  // top left
		};

		const uint32 indices[] = {
			0, 1, 2, // bottom right triangle
			2, 3, 0  // top left triangle
		};


		VertexArrayRef vao = VertexArray::Create();
		VertexBufferRef vbo = VertexBuffer::Create(data, sizeof(float) * 16);
		const IndexBufferRef ibo = IndexBuffer::Create(indices, 6);

		static const BufferLayout layout = {
			{ ShaderDataType::Float2 },
			{ ShaderDataType::Float2 }
		};

		vbo->SetLayout(layout);

		vao->AddVertexBuffer(vbo);
		vao->SetIndexBuffer(ibo);

		return new Mesh(vao);
	}

	MeshRef Renderer::CreateCubeMesh() const
	{
		const float vertices[] = {

			// front

			-1.0f, -1.0f,  1.0f, /* front bottom left  0 */  0.0f,  0.0f,  1.0f, /* forward normal */
			-1.0f, -1.0f,  1.0f, /* front bottom left  1 */ -1.0f,  0.0f,  0.0f, /* left normal	   */
			-1.0f, -1.0f,  1.0f, /* front bottom left  2 */  0.0f, -1.0f,  0.0f, /* down normal	*/

			 1.0f, -1.0f,  1.0f, /* front bottom right 3 */  0.0f,  0.0f,  1.0f, /* forward normal */
			 1.0f, -1.0f,  1.0f, /* front bottom right 4 */  1.0f,  0.0f,  0.0f, /* right normal	*/
			 1.0f, -1.0f,  1.0f, /* front bottom right 5 */  0.0f, -1.0f,  0.0f, /* down normal	*/

			 1.0f,  1.0f,  1.0f, /* front top right	   6 */  0.0f,  0.0f,  1.0f, /* forward normal */
			 1.0f,  1.0f,  1.0f, /* front top right	   7 */  1.0f,  0.0f,  0.0f, /* right normal	*/
			 1.0f,  1.0f,  1.0f, /* front top right	   8 */  0.0f,  1.0f,  0.0f, /* up normal		*/

			-1.0f,  1.0f,  1.0f, /* front top left	   9 */  0.0f,  0.0f,  1.0f, /* forward normal */
			-1.0f,  1.0f,  1.0f, /* front top left	  10 */ -1.0f,  0.0f,  0.0f, /* left normal	*/
			-1.0f,  1.0f,  1.0f, /* front top left	  11 */  0.0f,  1.0f,  0.0f, /* up normal		*/

			// back	

			-1.0f, -1.0f, -1.0f, /* back bottom left  12 */  0.0f,  0.0f, -1.0f, /* backward normal */
			-1.0f, -1.0f, -1.0f, /* back bottom left  13 */ -1.0f,  0.0f,  0.0f, /* left normal	 */
			-1.0f, -1.0f, -1.0f, /* back bottom left  14 */  0.0f, -1.0f,  0.0f, /* down normal	 */

			 1.0f, -1.0f, -1.0f, /* back bottom right 15 */  0.0f,  0.0f, -1.0f, /* backward normal */
			 1.0f, -1.0f, -1.0f, /* back bottom right 16 */  1.0f,  0.0f,  0.0f, /* right normal	 */
			 1.0f, -1.0f, -1.0f, /* back bottom right 17 */  0.0f, -1.0f,  0.0f, /* down normal	 */

			 1.0f,  1.0f, -1.0f, /* back top right	  18 */  0.0f,  0.0f, -1.0f, /* backward normal */
			 1.0f,  1.0f, -1.0f, /* back top right	  19 */  1.0f,  0.0f,  0.0f, /* right normal	 */
			 1.0f,  1.0f, -1.0f, /* back top right	  20 */  0.0f,  1.0f,  0.0f, /* up normal		 */

			-1.0f,  1.0f, -1.0f, /* back top left	  21 */  0.0f,  0.0f, -1.0f, /* backward normal */
			-1.0f,  1.0f, -1.0f, /* back top left	  22 */ -1.0f,  0.0f,  0.0f, /* left normal	 */
			-1.0f,  1.0f, -1.0f, /* back top left	  23 */  0.0f,  1.0f,  0.0f  /* up normal		 */

		};

		const uint32 indices[] = {

			// front
			 0,  3,  6,
			 6,  9,  0,

			 // right
			  4, 16, 19,
			 19,  7,  4,

			 // back
			 21, 18, 15,
			 15, 12, 21,

			 // left
			 13,  1, 10,
			 10, 22, 13,

			 // bottom
			 14, 17, 5,
			  5,  2, 14,

			  // top
			  11,  8, 20,
			  20, 23, 11

		};

		VertexArrayRef vao = VertexArray::Create();
		VertexBufferRef vbo = VertexBuffer::Create(vertices, sizeof(float) * 144);
		const IndexBufferRef ibo = IndexBuffer::Create(indices, 36);

		static const BufferLayout layout = {
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float3 }
		};

		vbo->SetLayout(layout);

		vao->AddVertexBuffer(vbo);
		vao->SetIndexBuffer(ibo);

		return new Mesh(vao);
	}

	MeshRef Renderer::CreateSphereMesh(uint32 segments) const
	{
		const float PI = 3.14159265359f;
		segments += 1;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvCoords;
		std::vector<glm::vec3> normals;
		std::vector<uint32> indices;


		for (uint32 y = 0; y < segments; ++y)
		{
			for (uint32 x = 0; x < segments; ++x)
			{
				float segmentX = static_cast<float>(x) / static_cast<float>(segments);
				float segmentY = static_cast<float>(y) / static_cast<float>(segments);

				float posX = std::cos(segmentX * 2.0f * PI) * std::sin(segmentX * PI);
				float posY = std::cos(segmentY * PI);
				float posZ = std::sin(segmentX * 2.0f * PI) * std::sin(segmentY * PI);

				positions.push_back(glm::vec3(posX, posY, posZ));
				uvCoords.push_back(glm::vec2(segmentX, segmentY));
				normals.push_back(glm::vec3(posX, posY, posZ));
			}
		}

		bool oddRow = false;
		for (uint32 y = 0; y < segments - 1; ++y)
		{
			if (!oddRow)
			{
				for (uint32 x = 0; x < segments; ++x)
				{
					indices.push_back(y * (segments + 1) + x);
					indices.push_back((y + 1) * (segments + 1) + x);
				}
			}
			else
			{
				for (int32 x = int32(segments); x >= 0; --x)
				{
					indices.push_back((y + 1) * (segments + 1) + x);
					indices.push_back(y * (segments + 1) + x);
				}
			}

			oddRow = !oddRow;
		}

		uint32 indexCount = static_cast<uint32>(indices.size());

		std::vector<float> data;
		data.reserve(positions.size() + uvCoords.size() + normals.size());

		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);

			data.push_back(uvCoords[i].x);
			data.push_back(uvCoords[i].y);

			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}

		VertexArrayRef vao = VertexArray::Create();
		VertexBufferRef vbo = VertexBuffer::Create(data.data(), static_cast<uint32>(sizeof(float) * data.size()));
		const IndexBufferRef ibo = IndexBuffer::Create(indices.data(), static_cast<uint32>(indices.size()));

		static const BufferLayout layout = {
			{ ShaderDataType::Float3 },
			{ ShaderDataType::Float2 },
			{ ShaderDataType::Float3 }
		};

		vbo->SetLayout(layout);

		vao->AddVertexBuffer(vbo);
		vao->SetIndexBuffer(ibo);

		return new Mesh(vao);

	}

}