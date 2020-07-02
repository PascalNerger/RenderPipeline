#include "pch.h"

#include <iostream>

#include "PBR/RHIOpenGL/OpenGL.h"
#include "PBR/RHIOpenGL/OpenGLShader.h"

namespace EngineCore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGLShader ////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	OpenGLShader::OpenGLShader(const std::string& name, const ShaderSource& source)
		: Handle(CreateShader(source))
		, Name(name)
		, UniformLocationCache()
	{
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(Handle);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(Handle);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::BindBlock(const std::string& name, const uint32 blockBinding)
	{
		glUniformBlockBinding(Handle, GetUniformBlockIndex(name), blockBinding);
	}

	void OpenGLShader::SetUniform1i(const std::string& name, const int32 value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetUniform2i(const std::string& name, const int32 f0, const int32 f1)
	{
		glUniform2i(GetUniformLocation(name), f0, f1);
	}

	void OpenGLShader::SetUniform3i(const std::string& name, const int32 f0, const int32 f1, const int32 f2)
	{
		glUniform3i(GetUniformLocation(name), f0, f1, f2);
	}

	void OpenGLShader::SetUniform4i(const std::string& name, const int32 f0, const int32 f1, const int32 f2, const int32 f3)
	{
		glUniform4i(GetUniformLocation(name), f0, f1, f2, f3);
	}

	void OpenGLShader::SetUniform1f(const std::string& name, const float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetUniform2f(const std::string& name, const float f0, const float f1)
	{
		glUniform2f(GetUniformLocation(name), f0, f1);
	}

	void OpenGLShader::SetUniform3f(const std::string& name, const float f0, const float f1, const float f2)
	{
		glUniform3f(GetUniformLocation(name), f0, f1, f2);
	}

	void OpenGLShader::SetUniform4f(const std::string& name, const float f0, const float f1, const float f2, const float f3)
	{
		glUniform4f(GetUniformLocation(name), f0, f1, f2, f3);
	}

	void OpenGLShader::SetUniformMat3f(const std::string& name, const float* data)
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, data);
	}

	void OpenGLShader::SetUniformMat4f(const std::string& name, const float* data)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, data);
	}

	uint32 OpenGLShader::CompileShader(const std::string& source, const uint32 type) const
	{
		const char* src = source.c_str();

		const uint32 id = glCreateShader(type);
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int32 result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			int32 length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

			char* message = static_cast<char*>(alloca(length * sizeof(char)));
			glGetShaderInfoLog(id, length, &length, message);

			std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
			std::cout << message << std::endl;

			glDeleteShader(id);
			return 0;
		}

		return id;
	}

	uint32 OpenGLShader::CreateShader(const ShaderSource& source) const
	{
		const uint32 program = glCreateProgram();

		const uint32 vs = CompileShader(source.VertexSource, GL_VERTEX_SHADER);
		const uint32 fs = CompileShader(source.FragmentSource, GL_FRAGMENT_SHADER);

		glAttachShader(program, vs);
		glAttachShader(program, fs);

		glLinkProgram(program);

		int32 success = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (success == GL_FALSE)
		{
			int32 length = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

			char* message = static_cast<char*>(alloca(length * sizeof(char)));
			glGetProgramInfoLog(program, length, &length, &message[0]);

			std::cout << "Failed to link program! " << std::endl;
			std::cout << message << std::endl;

			glDeleteProgram(program);
			return program;
		}

		glValidateProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}

	int32 OpenGLShader::GetUniformLocation(const std::string& name)
	{
		if (UniformLocationCache.find(name) != UniformLocationCache.end())
			return UniformLocationCache[name];

		const int32 location = glGetUniformLocation(Handle, name.c_str());
		if (location == -1)
			std::cout << "Warning: Uniform " << name << " does not exist!" << std::endl;

		UniformLocationCache[name] = location;
		return location;
	}

	int32 OpenGLShader::GetUniformBlockIndex(const std::string& name)
	{
		if (UniformBlockIndexCache.find(name) != UniformBlockIndexCache.end())
			return UniformBlockIndexCache[name];

		const int32 index = glGetUniformBlockIndex(Handle, name.c_str());
		if (index == -1)
			std::cout << "Warning: Uniform block " << name << " does not exist!" << std::endl;

		UniformBlockIndexCache[name] = index;
		return index;
	}

}
