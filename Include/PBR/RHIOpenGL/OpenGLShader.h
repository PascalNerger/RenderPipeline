#pragma once

#include <string>
#include <unordered_map>

#include "PBR/RHI/Shader.h"
#include "PBR/Core/BaseTypes.h"


namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLShader ////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class OpenGLShader : public Shader
	{

	public:

		OpenGLShader(const std::string& name, const ShaderSource& source);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void BindBlock(const std::string& name, const uint32 blockBinding) override;

		virtual void SetUniform1i(const std::string& name, const int32 value) override;
		virtual void SetUniform2i(const std::string& name, const int32 i0, const int32 i1) override;
		virtual void SetUniform3i(const std::string& name, const int32 i0, const int32 i1, const int32 i2) override;
		virtual void SetUniform4i(const std::string& name, const int32 i0, const int32 i1, const int32 i2, const int32 i3) override;

		virtual void SetUniform1f(const std::string& name, const float value) override;
		virtual void SetUniform2f(const std::string& name, const float f0, const float f1) override;
		virtual void SetUniform3f(const std::string& name, const float f0, const float f1, const float f2) override;
		virtual void SetUniform4f(const std::string& name, const float f0, const float f1, const float f2, const float f3) override;

		virtual void SetUniformMat3f(const std::string& name, const float* data) override;
		virtual void SetUniformMat4f(const std::string& name, const float* data) override;

		virtual const std::string& GetName() const override { return Name; }

	private:

		uint32 CompileShader(const std::string& source, const uint32 type) const;
		uint32 CreateShader(const ShaderSource& source) const;

		int32 GetUniformLocation(const std::string& name);
		int32 GetUniformBlockIndex(const std::string& name);

	private:

		uint32 Handle;
		std::string Name;

		std::unordered_map<std::string, int32> UniformLocationCache;
		std::unordered_map<std::string, int32> UniformBlockIndexCache;

	};

}