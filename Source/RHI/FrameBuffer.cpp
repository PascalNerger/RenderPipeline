#include "pch.h"

#include "PBR/Renderer/Renderer.h"
#include "PBR/RHIOpenGL/OpenGLFrameBuffer.h"


namespace EngineCore
{

	Ref<FrameBuffer> FrameBuffer::Create(const uint32 width, const uint32 height, const FrameBufferCreateInfo& createInfo)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLFrameBuffer(width, height, createInfo);

		default:
			break;
		}

		return nullptr;
	}

}