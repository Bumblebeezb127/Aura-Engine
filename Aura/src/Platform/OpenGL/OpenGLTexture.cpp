#include "arpch.h"
#include "OpenGLTexture.h"

#include <stb_image.h>

#include <glad/glad.h>

namespace Aura
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path):
		m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		AR_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;
		
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, GL_RGBA8, width, height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		//
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
		
		//if (data)
		//{
		//	glGenTextures(1, &m_RendererID);
		//	glBindTexture(GL_TEXTURE_2D, m_RendererID);

		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//	GLenum format = 0;
		//	if (channels == 1)
		//		format = GL_RED;
		//	else if (channels == 3)
		//		format = GL_RGB;
		//	else if (channels == 4)
		//		format = GL_RGBA;

		//	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//	glGenerateMipmap(GL_TEXTURE_2D);

		//	stbi_image_free(data);
		//}
		//else
		//{
		//	AR_CORE_ERROR("Failed to load texture: {0}", path);
		//}
	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
	/*	glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);*/
		glBindTextureUnit(slot, m_RendererID);
	}
}