#include "Sandbox2D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/ext/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 


Sandbox2D::Sandbox2D():
	Aura::Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	AR_PROFILE_FUNCTION();

	m_CheckerboardTexture = Aura::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	AR_PROFILE_FUNCTION();
}

float titlingFactor = 5.0f;
float rotation = 0.0f;
float pressValue = 5.0f;
void Sandbox2D::OnImGuiRender()
{
	AR_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::SliderFloat("Tiling Factor", &titlingFactor, 1.0f, 10.0f);
	ImGui::SliderFloat("Rotation", &rotation, -180.0f, 180.0f);
	ImGui::InputFloat("Rotation", &pressValue, 5.0f, 100000.0f);


	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", Aura::Renderer2D::GetStats().DrawCalls);
	ImGui::Text("Quads: %d", Aura::Renderer2D::GetStats().QuadCount);
	ImGui::Text("Vertices: %d", Aura::Renderer2D::GetStats().GetTotalVertexCount());
	ImGui::Text("Indices: %d", Aura::Renderer2D::GetStats().GetTotalIndexCount());
	ImGui::End();
}

void Sandbox2D::OnUpdate(Aura::Timestep timestep)
{
	AR_PROFILE_FUNCTION();
	{
		AR_PROFILE_SCOPE("Camera Update");
		m_CameraController.OnUpdate(timestep);
	}

	Aura::Renderer2D::ResetStats();
	{
		AR_PROFILE_SCOPE("Render Prep");
		Aura::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Aura::RenderCommand::Clear();
	}


	{
		AR_PROFILE_SCOPE("Render Draw");
		//==================================draw===========================================
		Aura::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Aura::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f, 0.0 }, { 1.0f, 1.0f }, 45.0f,{1,0,1,1});
		Aura::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Aura::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Aura::Renderer2D::DrawQuad({0.0f,0.0f , -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, titlingFactor);
		Aura::Renderer2D::DrawQuad({ -0.5f, -0.5f , 1.0f }, { 1.0f, 1.0f }, m_CheckerboardTexture, 20);
		Aura::Renderer2D::DrawRotatedQuad({ -1.0f, -1.5f , 1.0f }, { 1.0f, 1.0f }, rotation, m_CheckerboardTexture, 20);
		
		//Aura::Renderer2D::EndScene();

		//Aura::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for(float y = -5.0f; y <= pressValue; y += 0.5f) {
			for(float x = -5.0f; x <= pressValue; x += 0.5f) {
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f };
				Aura::Renderer2D::DrawQuad({ x,y }, { 0.45f, 0.45f }, color);
			}
		}
		Aura::Renderer2D::EndScene();
		//==================================end-draw=======================================

	}
}

void Sandbox2D::OnEvent(Aura::Event& event)
{
	m_CameraController.OnEvent(event);
}
