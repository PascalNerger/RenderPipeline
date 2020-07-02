#include "pch.h"

#include "PBR/RHI/VertexArray.h"
#include "PBR/RHIOpenGL/OpenGLVertexArray.h"

#include "PBR/Renderer/Renderer.h"


namespace EngineCore
{

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLVertexArray();

		default:
			break;
		}

		return nullptr;
	}

}