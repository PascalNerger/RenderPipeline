#pragma once

#include <string>

#include "PBR/Core/RefCounting.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderType //////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class ShaderType : uint8
	{

		None			= 0,
		Vertex			= 1,
		Tessellation	= 2,
		Geometry		= 3,
		Fragment		= 4

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ShaderSource ////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct ShaderSource
	{

		std::string VertexSource;
		std::string FragmentSource;

	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Shader //////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct Shader : public RefCountedObject
	{

		virtual ~Shader() { }

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void BindBlock(const std::string& name, const uint32 blockBinding) = 0;

		virtual void SetUniform1i(const std::string& name, const int32 value) = 0;
		virtual void SetUniform2i(const std::string& name, const int32 i0, const int32 i1) = 0;
		virtual void SetUniform3i(const std::string& name, const int32 i0, const int32 i1, const int32 i2) = 0;
		virtual void SetUniform4i(const std::string& name, const int32 i0, const int32 i1, const int32 i2, const int32 i3) = 0;

		virtual void SetUniform1f(const std::string& name, const float value) = 0;
		virtual void SetUniform2f(const std::string& name, const float f0, const float f1) = 0;
		virtual void SetUniform3f(const std::string& name, const float f0, const float f1, const float f2) = 0;
		virtual void SetUniform4f(const std::string& name, const float f0, const float f1, const float f2, const float f3) = 0;

		virtual void SetUniformMat3f(const std::string& name, const float* data) = 0;
		virtual void SetUniformMat4f(const std::string& name, const float* data) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& name, const ShaderSource& source);

	};

	using ShaderRef = Ref<Shader>;

}