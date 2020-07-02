#pragma once

#include <string>
#include <unordered_map>

#include "PBR/Engine/Material.h"


namespace EngineCore
{

	class MaterialLibrary
	{

	public:

		MaterialLibrary();
		~MaterialLibrary();

		void Add(const std::string& name, const MaterialRef& material);
		MaterialRef Get(const std::string& name);
		bool Exists(const std::string& name) const;

		void Save(const MaterialRef& material) const;
		void SaveAll() const;

		void Load(const std::string& file);

	private:

		std::unordered_map<std::string, MaterialRef> MaterialMap;

	};

}