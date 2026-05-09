#pragma once
#include "Aura/Core/Timestep.h"
#include "Aura/Events/Event.h"
#include "Aura/Events/MouseEvent.h"
#include "Aura/Events/ApplicationEvent.h"
#include "Aura/Renderer/OrthographicCamera.h"
#include "Aura/Core/Input.h"

namespace Aura
{
		class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);
		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
		
		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		float m_CameraRotation = 0.0f; // In degrees, in the anti-clockwise direction
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
		};
}