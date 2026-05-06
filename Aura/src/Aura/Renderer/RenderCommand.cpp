#include "arpch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Aura
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}
