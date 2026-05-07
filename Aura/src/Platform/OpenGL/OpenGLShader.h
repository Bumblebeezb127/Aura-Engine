#pragma once
#include "Aura/Renderer/Shader.h"


#include <glm/gtc/type_ptr.hpp>
namespace Aura
{
		class OpenGLShader: public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();


		virtual void Bind() const;
		virtual void Unbind() const;

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

		void UploadUniformFloat(const std::string& name, const float& value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformInt(const std::string& name, const int& value);
	private:
		uint32_t m_RendererID;
		};
}