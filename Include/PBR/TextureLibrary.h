#pragma once

#include <string>
#include <unordered_map>

#include "PBR/RHI/Texture.h"


namespace EngineCore
{

	class TextureLibrary
	{

	public:

		TextureLibrary();
		~TextureLibrary();

		void Add(const std::string& name, const TextureRef& texture);

		TextureRef Get(const std::string& name);
		Texture2DRef Get2D(const std::string& name);
		TextureCubeRef GetCube(const std::string& name);

		bool Exists(const std::string& name);

		Texture2DRef Load2D(const std::string& file, const TextureCreateInfo createInfo = TextureCreateInfo());
		Texture2DRef Load2DHdr(const std::string& file, const TextureCreateInfo createInfo = TextureCreateInfo());
		
		// TODO:
		//void LoadCube();

	private:

		std::string GetFileName(const std::string& file) const;

	private:

		static TextureLibrary* GetInstance() { return Instance; }

	private:

		std::unordered_map<std::string, TextureRef> TextureMap;

	private:
		
		static TextureLibrary* Instance;

	};

}