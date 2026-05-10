#include "arpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Aura
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type)
		{
		case Aura::ShaderDataType::Float:		return GL_FLOAT;
		case Aura::ShaderDataType::Float2:	return GL_FLOAT;
		case Aura::ShaderDataType::Float3:	return GL_FLOAT;
		case Aura::ShaderDataType::Float4:	return GL_FLOAT;
		case Aura::ShaderDataType::Mat3:		return GL_FLOAT;
		case Aura::ShaderDataType::Mat4:		return GL_FLOAT;
		case Aura::ShaderDataType::Int:		return GL_INT;
		case Aura::ShaderDataType::Int2:		return GL_INT;
		case Aura::ShaderDataType::Int3:		return GL_INT;
		case Aura::ShaderDataType::Int4:		return GL_INT;
		case Aura::ShaderDataType::Bool:		return GL_BOOL;

		}
		AR_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		AR_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		AR_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		AR_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		AR_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		AR_PROFILE_FUNCTION();

		AR_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.
				GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		AR_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}