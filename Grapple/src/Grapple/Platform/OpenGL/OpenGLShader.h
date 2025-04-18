#pragma once

#include "Grapple/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <string>
#include <string_view>

namespace Grapple
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::filesystem::path& path);
		~OpenGLShader();
	public:
		virtual void Bind() override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, glm::vec2 value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetIntArray(const std::string& name, const int* values, uint32_t count) override;
		virtual void SetMatrix4(const std::string& name, const glm::mat4& matrix) override;
	private:
		struct ShaderProgram
		{
			std::string Source;
			uint32_t Type;

			ShaderProgram(std::string_view source, uint32_t type)
				: Source(source), Type(type) {}
		};

		std::vector<ShaderProgram> PreProcess(std::string_view source);
		void Compile(const std::filesystem::path& path, std::string_view source);
	private:
		uint32_t m_Id;
	};
}