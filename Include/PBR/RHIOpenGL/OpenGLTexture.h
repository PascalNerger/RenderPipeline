#pragma once

#include "PBR/RHI/Texture.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLTextureUtils //////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLTextureUtils
	{

	public:

		static uint32 TextureDataTypeToGl(const TextureDataType type);
		static uint32 TextureFormatToGl(const TextureFormat format);
		static uint32 TextureMinFilterToGl(const TextureFilter filter);
		static uint32 TextureMagFilterToGl(const TextureFilter filter);
		static uint32 TextureWrapToGl(const TextureWrap wrap);

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLTexture2D /////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLTexture2D : public Texture2D
	{

	public:

		OpenGLTexture2D(const uint32 width, const uint32 height, const TextureCreateInfo& createInfo = TextureCreateInfo());
		OpenGLTexture2D(const uint32 width, const uint32 height, const void* data, const TextureCreateInfo& createInfo = TextureCreateInfo());
		virtual ~OpenGLTexture2D();

		virtual void Bind(uint32 slot = 0) const override;
		virtual void Unbind() const override;

		// TODO:
		[[deprecated("Not working, texture is not drawn. Use constructor with data instead!")]]
		virtual void SetData(const byte* data) override;


		virtual uint32 GetWidth() const override { return Width; }
		virtual uint32 GetHeight() const override { return Height; }

		virtual uint64 GetShaderHandle() const override { return ShaderHandle; }

		inline uint32 GetHandle() const { return Handle; }

	private:

		void Create(const uint32 width, const uint32 height, const void* data, const TextureCreateInfo& createInfo);

	private:

		uint32 Handle;
		uint64 ShaderHandle;

		uint32 Width;
		uint32 Height;

		TextureCreateInfo CreateInfo;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLTextureCube ///////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLTextureCube : public TextureCube
	{

	public:

		OpenGLTextureCube(const uint32 width, const uint32 height, const TextureCreateInfo& createInfo = TextureCreateInfo());
		OpenGLTextureCube(const uint32 width, const uint32 height, const CubeMapCreateData& data, const TextureCreateInfo& createInfo = TextureCreateInfo());
		virtual ~OpenGLTextureCube();

		virtual void Bind(uint32 slot = 0) const override;
		virtual void Unbind() const override;

		virtual void SetData(const byte* data, const CubeMapOrientation orientation) override;

		virtual uint32 GetWidth() const override { return Width; }
		virtual uint32 GetHeight() const override { return Height; }

		virtual uint64 GetShaderHandle() const override { return ShaderHandle; }

		inline uint32 GetHandle() const { return Handle; }

	private:

		void Create(const uint32 width, const uint32 height, const CubeMapCreateData& data, const TextureCreateInfo& createInfo);

	private:

		uint32 Handle;
		uint64 ShaderHandle;

		uint32 Width;
		uint32 Height;

	};

}