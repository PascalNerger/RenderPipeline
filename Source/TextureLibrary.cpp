#include "pch.h"

#include <stb_image.h>
#include "PBR/TextureLibrary.h"


namespace EngineCore
{

	TextureLibrary* TextureLibrary::Instance = nullptr;

	TextureLibrary::TextureLibrary()
	{

	}

	TextureLibrary::~TextureLibrary()
	{

	}

	void TextureLibrary::Add(const std::string& name, const TextureRef& texture)
	{
		if (Exists(name))
		{
			assert(false);
			return;
		}

		TextureMap[name] = texture;
	}

	TextureRef TextureLibrary::Get(const std::string& name)
	{
		if (!Exists(name))
		{
			assert(false);
			return nullptr;
		}

		return TextureMap[name];
	}

	Texture2DRef TextureLibrary::Get2D(const std::string& name)
	{
		return RefDynamicCast<Texture2D>(Get(name));
	}

	TextureCubeRef TextureLibrary::GetCube(const std::string& name)
	{
		return RefDynamicCast<TextureCube>(Get(name));
	}

	bool TextureLibrary::Exists(const std::string& name)
	{
		return TextureMap.find(name) != TextureMap.end();
	}

	Texture2DRef TextureLibrary::Load2D(const std::string& file, const TextureCreateInfo createInfo)
	{
		int32 width;
		int32 height;
		int32 channels;

		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pixels = stbi_load(file.c_str(), &width, &height, &channels, STBI_rgb);

		// TODO: If textures are not specified as SRGB,
		// the texture has to be converted to linear color space in the shader!

		Texture2DRef texture = Texture2D::Create(width, height, pixels, createInfo);
		stbi_image_free(pixels);

		Add(GetFileName(file), RefCast<Texture>(texture));
		return texture;
	}

	Texture2DRef TextureLibrary::Load2DHdr(const std::string& file, const TextureCreateInfo createInfo)
	{
		int32 width;
		int32 height;
		int32 channels;

		stbi_set_flip_vertically_on_load(true);
		float* pixels = stbi_loadf(file.c_str(), &width, &height, &channels, STBI_rgb);

		Texture2DRef texture = Texture2D::Create(width, height, pixels, createInfo);
		stbi_image_free(pixels);

		Add(GetFileName(file), RefCast<Texture>(texture));
		return texture;
	}

	std::string TextureLibrary::GetFileName(const std::string& file) const
	{
		auto lastSlash = file.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? lastSlash : lastSlash + 1;

		auto lastDot = file.rfind('.');
		auto count = lastDot == std::string::npos ? file.size() - lastSlash : lastDot - lastSlash;

		std::string name = file.substr(lastSlash, count);
		return name;
	}

}