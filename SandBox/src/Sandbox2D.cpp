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
	m_CheckerboardTexture = Aura::Texture2D::Create("assets/textures/Checkerboard.png");
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
	Aura::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Aura::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Aura::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
	Aura::Renderer2D::DrawQuad({ 0.0f, 0.0f , -0.1f}, { 10.0f, 10.0f }, m_CheckerboardTexture);

	Aura::Renderer2D::EndScene();
	//==================================end-draw=======================================
}

void Sandbox2D::OnEvent(Aura::Event& event)
{
	m_CameraController.OnEvent(event);
}
