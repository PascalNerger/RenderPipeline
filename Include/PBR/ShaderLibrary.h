#pragma once

#include <string>
#include <unordered_map>

#include "RHI/Shader.h"


namespace EngineCore
{

	class ShaderLibrary
	{

	public:

		ShaderLibrary();
		~ShaderLibrary();

		void Add(const ShaderRef& shader);
		ShaderRef& Get(const std::string& name);
		bool Exists(const std::string& name) const;

		void Load(const std::string& name, const ShaderSource& source);
		void Load(const std::string& file);

	private:

		std::string ReadFile(const std::string& file) const;
		std::string GetFileName(const std::string& file) const;

		ShaderType GetShaderTypeFromString(const std::string& value) const;

	public:

		static ShaderLibrary* GetInstance() { return Instance; }

	private:

		std::unordered_map<std::string, ShaderRef> ShaderMap;

	private:

		static ShaderLibrary* Instance;

	};

}