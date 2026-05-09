#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/ext/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 


Sandbox2D::Sandbox2D():
	Aura::Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	//==================================square===========================
	m_SquareVA = Aura::VertexArray::Create();
	float squareVertices[3 * 4] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	Aura::Ref<Aura::VertexBuffer> squareVB;
	squareVB.reset(Aura::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	squareVB->SetLayout({
		{ Aura::ShaderDataType::Float3, "position" }
		});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0,1,2, 2,3,0 };
	Aura::Ref<Aura::IndexBuffer> squareIB;
	squareIB.reset(Aura::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	m_SquareVA->SetIndexBuffer(squareIB);

	m_FlatColorShader = Aura::Shader::Create("assets/shaders/FlatColor.glsl");



}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnUpdate(Aura::Timestep timestep)
{
	m_CameraController.OnUpdate(timestep);

	Aura::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Aura::RenderCommand::Clear();


	//==================================draw===========================================
	Aura::Renderer::BeginScene(m_CameraController.GetCamera());

	std::dynamic_pointer_cast<Aura::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Aura::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);

	m_FlatColorShader->Bind();
	Aura::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));



	Aura::Renderer::EndScene();
	//==================================end-draw=======================================
}

void Sandbox2D::OnEvent(Aura::Event& event)
{
	m_CameraController.OnEvent(event);
}
