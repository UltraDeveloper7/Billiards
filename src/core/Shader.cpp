#include "../stdafx.h"
#include "Shader.hpp"

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path) : id_{}
{
	const auto vertex = std::filesystem::current_path() / "src/shaders" / vertex_path;
	const auto fragment = std::filesystem::current_path() / "src/shaders" / fragment_path;

	const auto vertex_shader = LoadShader(GL_VERTEX_SHADER, vertex.string());
	const auto fragment_shader = LoadShader(GL_FRAGMENT_SHADER, fragment.string());

	unsigned geometry_shader = 0;

	if (!geometry_path.empty())
	{
		const auto geometry = std::filesystem::current_path() / "src/shaders" / geometry_path;
		geometry_shader = LoadShader(GL_GEOMETRY_SHADER, geometry.string());
	}

	LinkProgram(vertex_shader, fragment_shader, geometry_shader);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

std::string Shader::LoadShaderSource(const std::string& path) const
{
	std::ifstream file(path, std::ios::in | std::ios::binary);
    const size_t size = std::filesystem::file_size(path);
    std::string source(size, '\0');

    file.read(source.data(), static_cast<long long>(size));
	file.close();
	
	return source;
}

GLuint Shader::LoadShader(const unsigned type, const std::string& path) const
{
	GLint success;

	const auto shader = glCreateShader(type);
	const auto str_src = LoadShaderSource(path);
	const auto src = str_src.c_str();

	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char info_log[512];
		glGetShaderInfoLog(shader, 512, nullptr, info_log);
		throw std::exception(info_log);
	}

	return shader;
}

void Shader::LinkProgram(const unsigned vertex, const unsigned fragment, const unsigned geometry)
{
	GLint success{0};

	id_ = glCreateProgram();

	glAttachShader(id_, vertex);
	glAttachShader(id_, fragment);

	if (geometry != 0)
		glAttachShader(id_, geometry);

	glLinkProgram(id_);
	glValidateProgram(id_);

	glGetProgramiv(id_, GL_LINK_STATUS, &success);
	if (!success)
	{
		char info_log[512];
		glGetProgramInfoLog(id_, 512, nullptr, info_log);
		throw std::exception(info_log);
	}

	glUseProgram(0);
}

void Shader::Bind() const
{
	glUseProgram(id_);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetMat4(const glm::mat4& m, const std::string& name) const
{
	const auto my_loc = glGetUniformLocation(id_, name.c_str());
	glUniformMatrix4fv(my_loc, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::SetVec3(const glm::vec3& v, const std::string& name) const
{
	const auto my_loc = glGetUniformLocation(id_, name.c_str());
	glProgramUniform3fv(id_, my_loc, 1, glm::value_ptr(v));
}

void Shader::SetFloat(const float s, const std::string& name) const
{
	const auto my_loc = glGetUniformLocation(id_, name.c_str());
	glUniform1f(my_loc, s);
}

void Shader::SetInt(const int n, const std::string& name) const
{
	const auto my_loc = glGetUniformLocation(id_, name.c_str());
	glUniform1i(my_loc, n);
}

void Shader::SetBool(const bool c, const std::string& name) const
{
	const auto my_loc = glGetUniformLocation(id_, name.c_str());
	glUniform1i(my_loc, c);
}
