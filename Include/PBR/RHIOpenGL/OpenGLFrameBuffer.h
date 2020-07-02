#pragma once

#include "PBR/RHI/FrameBuffer.h"
#include "PBR/RHI/Texture.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLFrameBuffer ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLFrameBuffer : public FrameBuffer
	{

	public:

		OpenGLFrameBuffer(const uint32 width, const uint32 height, const FrameBufferCreateInfo& createInfo = FrameBufferCreateInfo());
		virtual ~OpenGLFrameBuffer();


		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void Attach(const Ref<Texture2D>& texture, const TextureAttachment attachment = TextureAttachment::Color) override;
		virtual void Attach(const Ref<TextureCube>& cubeMap, const CubeMapOrientation orientation, const TextureAttachment attachment = TextureAttachment::Color, const uint32 mipLevel = 0) override;

		virtual void Resize(const uint32 width, const uint32 height) override;

		virtual void Clear() override;

		virtual uint32 GetWidth() const { return Width; }
		virtual uint32 GetHeight() const { return Height; }

	private:

		uint32 TextureAttachmentToGl(const TextureAttachment attachment) const;
		uint32 CubeMapOrientationToGl(const CubeMapOrientation orientation) const;

	private:

		uint32 FrameHandle;
		uint32 DepthHandle;

		uint32 Width;
		uint32 Height;

	};

}