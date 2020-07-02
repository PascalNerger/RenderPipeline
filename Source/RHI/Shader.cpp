#include "pch.h"
#include "PBR/RHI/Shader.h"

#include "PBR/Renderer/Renderer.h"
#include "PBR/RHIOpenGL/OpenGLShader.h"


namespace EngineCore
{

	Ref<Shader> Shader::Create(const std::string& name, const ShaderSource& source)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLShader(name, source);

		default:
			break;
		}

		return nullptr;
	}

}