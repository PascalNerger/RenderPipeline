#include "pch.h"

#include "PBR/RHIOpenGl/OpenGl.h"
#include "PBR/RHIOpenGl/OpenGLFrameBuffer.h"
#include "PBR/RHIOpenGL/OpenGLTexture.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLFrameBuffer ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLFrameBuffer::OpenGLFrameBuffer(const uint32 width, const uint32 height, const FrameBufferCreateInfo& createInfo)
		: FrameHandle(0), DepthHandle(0), Width(width), Height(height)
	{

		glGenFramebuffers(1, &FrameHandle);
		glGenRenderbuffers(1, &DepthHandle);

		glBindRenderbuffer(GL_RENDERBUFFER, DepthHandle);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, FrameHandle);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthHandle);

		if (!createInfo.RenderColorBuffer)
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &FrameHandle);
		glDeleteRenderbuffers(1, &DepthHandle);
	}

	void OpenGLFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FrameHandle);
		glViewport(0, 0, Width, Height);
	}

	void OpenGLFrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Attach(const Ref<Texture2D>& texture, const TextureAttachment attachment)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FrameHandle);
		glFramebufferTexture2D(GL_FRAMEBUFFER, TextureAttachmentToGl(attachment), GL_TEXTURE_2D, RefCast<OpenGLTexture2D>(texture)->GetHandle(), 0);
	}

	void OpenGLFrameBuffer::Attach(const Ref<TextureCube>& cubeMap, const CubeMapOrientation orientation, const TextureAttachment attachment, const uint32 mipLevel)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FrameHandle);
		glFramebufferTexture2D(GL_FRAMEBUFFER, TextureAttachmentToGl(attachment), CubeMapOrientationToGl(orientation), RefCast<OpenGLTextureCube>(cubeMap)->GetHandle(), mipLevel);
	}

	void OpenGLFrameBuffer::Resize(const uint32 width, const uint32 height)
	{
		Width = width;
		Height = height;

		glBindRenderbuffer(GL_RENDERBUFFER, DepthHandle);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

		glViewport(0, 0, width, height);
	}

	void OpenGLFrameBuffer::Clear()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	uint32 OpenGLFrameBuffer::TextureAttachmentToGl(const TextureAttachment attachment) const
	{
		switch (attachment)
		{
		case TextureAttachment::Color:		return GL_COLOR_ATTACHMENT0;
		case TextureAttachment::Color1:		return GL_COLOR_ATTACHMENT1;
		case TextureAttachment::Color2:		return GL_COLOR_ATTACHMENT2;
		case TextureAttachment::Color3:		return GL_COLOR_ATTACHMENT3;
		case TextureAttachment::Depth:		return GL_DEPTH_ATTACHMENT;
		case TextureAttachment::Stencil:	return GL_STENCIL_ATTACHMENT;

		default:
			break;
		}

		return 0;
	}

	uint32 OpenGLFrameBuffer::CubeMapOrientationToGl(const CubeMapOrientation orientation) const
	{
		switch (orientation)
		{
		case CubeMapOrientation::PositiveX: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		case CubeMapOrientation::NegativeX: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		case CubeMapOrientation::PositiveY: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		case CubeMapOrientation::NegativeY: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		case CubeMapOrientation::PositiveZ: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		case CubeMapOrientation::NegativeZ: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

		default:
			break;
		}

		return 0;
	}

}