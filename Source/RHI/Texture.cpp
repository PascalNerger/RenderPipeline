#include "pch.h"
#include "PBR/RHI/Texture.h"

#include "PBR/Renderer/Renderer.h"
#include "PBR/RHIOpenGL/OpenGLTexture.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Texture2d ///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Ref<Texture2D> Texture2D::Create(const uint32 width, const uint32 height, const TextureCreateInfo& createInfo)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLTexture2D(width, height, createInfo);

		default:
			break;
		}

		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const uint32 width, const uint32 height, const void* data, const TextureCreateInfo& createInfo)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLTexture2D(width, height, data, createInfo);

		default:
			break;
		}

		return nullptr;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// CubeMapTexture //////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Ref<TextureCube> TextureCube::Create(const uint32 width, const uint32 height, const TextureCreateInfo& createInfo)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLTextureCube(width, height, createInfo);

		default:
			break;
		}

		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(const uint32 width, const uint32 height, const CubeMapCreateData& data, const TextureCreateInfo& createInfo)
	{
		switch (Renderer::GetApi())
		{
		case RenderApi::OpenGL: return new OpenGLTextureCube(width, height, data, createInfo);

		default:
			break;
		}

		return nullptr;
	}

}