#include "pch.h"
#include "PBR/RHI/Context.h"

#include "PBR/RHIOpenGL/OpenGLContext.h"


namespace EngineCore
{

	RHIContext* RHIContext::Instance = nullptr;

	RHIContext::RHIContext()
	{
		assert(!Instance);
		Instance = this;
	}

	RHIContext* RHIContext::Create(const RenderApi api)
	{
		switch (api)
		{
		case RenderApi::OpenGL: return new OpenGLContext();

		default:
			break;
		}

		return nullptr;
	}

}