#include "pch.h"

#include "PBR/RHIOpenGL/OpenGL.h"
#include "PBR/RHIOpenGL/OpenGLTexture.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLTextureUtils //////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	uint32 OpenGLTextureUtils::TextureDataTypeToGl(const TextureDataType type)
	{
		switch (type)
		{
		case TextureDataType::UnsignedByte: return GL_UNSIGNED_BYTE;
		case TextureDataType::UnsignedInt:	return GL_UNSIGNED_INT;
		case TextureDataType::Float:		return GL_FLOAT;

		default:
			break;
		}

		return 0;
	}

	uint32 OpenGLTextureUtils::TextureFormatToGl(const TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::Rg:					return GL_RG;
		case TextureFormat::Rg16f:				return GL_RG16F;
		case TextureFormat::Rgb:				return GL_RGB;
		case TextureFormat::Rgb16f:				return GL_RGB16F;
		case TextureFormat::Rgb32f:				return GL_RGB32F;
		case TextureFormat::Rgba:				return GL_RGBA;
		case TextureFormat::Rgba16f:			return GL_RGBA16F;
		case TextureFormat::Rgba32f:			return GL_RGBA32F;
		case TextureFormat::Srgb:				return GL_SRGB;
			// case TextureFormat::Luminance:		return GL_LUMINANCE;
			// case TextureFormat::LuminanceAlpha:	return GL_LUMINANCE_ALPHA;

		case TextureFormat::Depth:				return GL_DEPTH_COMPONENT;
		case TextureFormat::Depth16:			return GL_DEPTH_COMPONENT16;
		case TextureFormat::Depth32:			return GL_DEPTH_COMPONENT32;

		default: break;
		}

		return 0;
	}

	uint32 OpenGLTextureUtils::TextureMinFilterToGl(const TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Nearest:	return GL_NEAREST;
		case TextureFilter::Linear:		return GL_LINEAR;
		//case TextureFilter::Trilinear:	return GL_LINEAR_MIPMAP_LINEAR;
		//case TextureFilter::Anisotropic: return -1;

		default: break;
		}

		return 0;
	}

	uint32 OpenGLTextureUtils::TextureMagFilterToGl(const TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Nearest:	return GL_NEAREST;
		case TextureFilter::Linear:		return GL_LINEAR;
		//case TextureFilter::Trilinear:	return GL_LINEAR;
		//case TextureFilter::Anisotropic: return -1;

		default: break;
		}

		return 0;
	}

	uint32 OpenGLTextureUtils::TextureWrapToGl(const TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Repeat:			return GL_REPEAT;
		case TextureWrap::Clamp:			return GL_CLAMP_TO_EDGE; // TODO: GL_CLAMP does not exist ?!
		case TextureWrap::ClampToEdge:		return GL_CLAMP_TO_EDGE;
		case TextureWrap::ClampToBorder:	return GL_CLAMP_TO_BORDER;

		default: break;
		}

		return 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLTexture2D /////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLTexture2D::OpenGLTexture2D(const uint32 width, const uint32 height, const TextureCreateInfo& createInfo)
		: Width(width), Height(height), CreateInfo(createInfo)
	{
		Create(width, height, nullptr, createInfo);
	}

	OpenGLTexture2D::OpenGLTexture2D(const uint32 width, const uint32 height, const void* data, const TextureCreateInfo& createInfo)
		: Width(width), Height(height), CreateInfo(createInfo)
	{
		Create(width, height, data, createInfo);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &Handle);
	}

	void OpenGLTexture2D::Bind(uint32 slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, Handle);
	}

	void OpenGLTexture2D::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture2D::SetData(const byte* data)
	{
		glBindTexture(GL_TEXTURE_2D, Handle);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, OpenGLTextureUtils::TextureFormatToGl(CreateInfo.Format), GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture2D::Create(const uint32 width, const uint32 height, const void* data, const TextureCreateInfo& createInfo)
	{
		glGenTextures(1, &Handle);
		glBindTexture(GL_TEXTURE_2D, Handle);

		if (createInfo.UseMipMaps)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, createInfo.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, createInfo.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, createInfo.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLTextureUtils::TextureWrapToGl(createInfo.Wrap));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLTextureUtils::TextureWrapToGl(createInfo.Wrap));

		if (createInfo.IsShadowTexture)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			OpenGLTextureUtils::TextureFormatToGl(createInfo.InternalFormat),
			width,
			height,
			0,
			OpenGLTextureUtils::TextureFormatToGl(createInfo.Format),
			OpenGLTextureUtils::TextureDataTypeToGl(createInfo.DataType),
			data
		);

		if (createInfo.UseMipMaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);

			if (createInfo.AnisotropyLevel > 0.0f)
			{
				float maxAnisotropy = 0.0f;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);

				float amount = std::min(maxAnisotropy, createInfo.AnisotropyLevel);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, amount);
			}
		}

		ShaderHandle = glGetTextureHandleARB(Handle);
		glMakeTextureHandleResidentARB(ShaderHandle);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLTextureCube ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLTextureCube::OpenGLTextureCube(const uint32 width, const uint32 height, const TextureCreateInfo& createInfo)
		: Handle(0), Width(width), Height(height)
	{
		Create(Width, height, CubeMapCreateData(), createInfo);
	}

	OpenGLTextureCube::OpenGLTextureCube(const uint32 width, const uint32 height, const CubeMapCreateData& data, const TextureCreateInfo& createInfo)
		: Handle(0), Width(width), Height(height)
	{
		Create(width, height, data, createInfo);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		glDeleteTextures(1, &Handle);
	}

	void OpenGLTextureCube::Bind(uint32 slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Handle);
	}

	void OpenGLTextureCube::Unbind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void OpenGLTextureCube::SetData(const byte* data, const CubeMapOrientation orientation)
	{
		// TODO:
	}

	void OpenGLTextureCube::Create(const uint32 width, const uint32 height, const CubeMapCreateData& data, const TextureCreateInfo& createInfo)
	{
		glGenTextures(1, &Handle);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Handle);

		if (createInfo.UseMipMaps)
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, createInfo.Filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
		else
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, createInfo.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, createInfo.Filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, OpenGLTextureUtils::TextureWrapToGl(createInfo.Wrap));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, OpenGLTextureUtils::TextureWrapToGl(createInfo.Wrap));

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.InternalFormat), width, height, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.Format), OpenGLTextureUtils::TextureDataTypeToGl(createInfo.DataType), data.Right);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.InternalFormat), width, height, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.Format), OpenGLTextureUtils::TextureDataTypeToGl(createInfo.DataType), data.Left);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.InternalFormat), width, height, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.Format), OpenGLTextureUtils::TextureDataTypeToGl(createInfo.DataType), data.Top);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.InternalFormat), width, height, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.Format), OpenGLTextureUtils::TextureDataTypeToGl(createInfo.DataType), data.Bottom);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.InternalFormat), width, height, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.Format), OpenGLTextureUtils::TextureDataTypeToGl(createInfo.DataType), data.Front);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.InternalFormat), width, height, 0, OpenGLTextureUtils::TextureFormatToGl(createInfo.Format), OpenGLTextureUtils::TextureDataTypeToGl(createInfo.DataType), data.Back);

		if (createInfo.UseMipMaps)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		ShaderHandle = glGetTextureHandleARB(Handle);
		glMakeTextureHandleResidentARB(ShaderHandle);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}