#include "pch.h"

#include "PBR/MaterialLibrary.h"


namespace EngineCore
{

	MaterialLibrary::MaterialLibrary()
	{

	}

	MaterialLibrary::~MaterialLibrary()
	{

	}

	void MaterialLibrary::Add(const std::string& name, const MaterialRef& material)
	{
		if (Exists(name))
		{
			assert(false);
			return;
		}

		MaterialMap[name] = material;
	}

	MaterialRef MaterialLibrary::Get(const std::string& name)
	{
		if (!Exists(name))
		{
			assert(false);
			return nullptr;
		}

		return MaterialMap[name];
	}

	bool MaterialLibrary::Exists(const std::string& name) const
	{
		return MaterialMap.find(name) != MaterialMap.end();
	}

	void MaterialLibrary::Save(const MaterialRef& material) const
	{

	}

	void MaterialLibrary::SaveAll() const
	{

	}

	void MaterialLibrary::Load(const std::string& file)
	{
		
	}

}