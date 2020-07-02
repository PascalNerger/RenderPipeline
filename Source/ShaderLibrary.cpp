#include "pch.h"

#include <fstream>
#include "PBR/ShaderLibrary.h"


namespace EngineCore
{

	ShaderLibrary* ShaderLibrary::Instance = nullptr;

	ShaderLibrary::ShaderLibrary()
	{
		if (Instance)
		{
			assert(false);
		}

		Instance = this;
	}

	ShaderLibrary::~ShaderLibrary()
	{

	}

	void ShaderLibrary::Add(const ShaderRef& shader)
	{
		if (Exists(shader->GetName()))
		{
			assert(false);
			return;
		}

		ShaderMap[shader->GetName()] = shader;
	}

	ShaderRef& ShaderLibrary::Get(const std::string& name)
	{
		if (!Exists(name))
		{
			assert(false);
		}

		return ShaderMap[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return ShaderMap.find(name) != ShaderMap.end();
	}

	void ShaderLibrary::Load(const std::string& name, const ShaderSource& source)
	{
		ShaderRef shader = Shader::Create(name, source);
		Add(shader);
	}

	void ShaderLibrary::Load(const std::string& file)
	{
		std::string source = ReadFile(file);
		std::unordered_map<ShaderType, std::string> shaderSources;

		const char* typeToken = "#type";
		std::size_t typeTokenLength = std::strlen(typeToken);
		std::size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			assert(eol != std::string::npos);

			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);

			assert(GetShaderTypeFromString(type) != ShaderType::None);

			std::size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);

			shaderSources[GetShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		ShaderSource resultSource;
		resultSource.VertexSource = shaderSources[ShaderType::Vertex];
		resultSource.FragmentSource = shaderSources[ShaderType::Fragment];

		Load(GetFileName(file), resultSource);
	}

	std::string ShaderLibrary::ReadFile(const std::string& file) const
	{
		std::string result;
		std::ifstream in(file, std::ios::in | std::ios::binary);

		if (!in)
		{
			assert(false);
			return "<error>";
		}

		in.seekg(0, std::ios::end);
		result.resize(in.tellg());

		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());

		in.close();
		return result;
	}

	std::string ShaderLibrary::GetFileName(const std::string& file) const
	{
		auto lastSlash = file.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? lastSlash : lastSlash + 1;

		auto lastDot = file.rfind('.');
		auto count = lastDot == std::string::npos ? file.size() - lastSlash : lastDot - lastSlash;

		std::string name = file.substr(lastSlash, count);
		return name;
	}

	ShaderType ShaderLibrary::GetShaderTypeFromString(const std::string& value) const
	{
		if (value == "vertex")
			return ShaderType::Vertex;
		else if (value == "tessellation")
			return ShaderType::Tessellation;
		else if (value == "geometry")
			return ShaderType::Geometry;
		else if (value == "fragment" || value == "pixel")
			return ShaderType::Fragment;

		return ShaderType::None;
	}

}