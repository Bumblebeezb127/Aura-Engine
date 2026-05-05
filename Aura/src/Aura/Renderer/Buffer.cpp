#include "arpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Aura {
    VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::None: AR_CORE_ASSERT(false, "RendererAPI::None is currently not support!"); return nullptr;
            case RendererAPI::OpenGL: return new OpenGLVertexBuffer(vertices, size);
        }

        AR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size) {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::None: AR_CORE_ASSERT(false, "RendererAPI::None is currently not support!"); return nullptr;
        case RendererAPI::OpenGL: return new OpenGLIndexBuffer(indices, size);
        }

        AR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}