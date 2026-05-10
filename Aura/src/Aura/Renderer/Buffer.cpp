#include "arpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Aura {
    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: AR_CORE_ASSERT(false, "RendererAPI::None is currently not support!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
        }

        AR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
    Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: AR_CORE_ASSERT(false, "RendererAPI::None is currently not support!"); return nullptr;
            case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices, size);
        }

        AR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None: AR_CORE_ASSERT(false, "RendererAPI::None is currently not support!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, count);
        }

        AR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}