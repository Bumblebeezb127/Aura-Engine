#pragma once

#include <Aura.h>

class Sandbox2D:public Aura::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnImGuiRender() override;

	void OnUpdate(Aura::Timestep timestep) override;
	void OnEvent(Aura::Event& event) override;
private:
	Aura::OrthographicCameraController m_CameraController;

	//Temp
	Aura::Ref<Aura::VertexArray> m_SquareVA;
	Aura::Ref<Aura::Shader> m_FlatColorShader;

	Aura::Ref<Aura::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor{ 0.8f, 0.2f, 0.3f, 1.0f };
};

