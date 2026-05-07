#include <Aura.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}


class ExampleLayer :public Aura::Layer {
public:
	ExampleLayer() :
		Layer("Example"), 
		m_Camera(-1.0f, 1.0f, -1.0f, 1.0f),
		m_CameraPosition(0.0f, 0.0f, 0.0f)
	{
		m_VertexArray.reset(Aura::VertexArray::Create());

		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		std::shared_ptr<Aura::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(Aura::VertexBuffer::Create(vertices, sizeof(vertices)));
		Aura::BufferLayout layout = {
			{Aura::ShaderDataType::Float3, "position"}
		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);


		uint32_t indices[3] = { 0,1,2 };
		std::shared_ptr<Aura::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(Aura::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
		//==================================square===========================
		m_SquareVA.reset(Aura::VertexArray::Create());
		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.2f, 0.0f,
			-0.5f,  0.2f, 0.0f
		};

		std::shared_ptr<Aura::VertexBuffer> squareVB;
		squareVB.reset(Aura::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Aura::ShaderDataType::Float3, "position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0,1,2, 2,3,0 };
		std::shared_ptr<Aura::IndexBuffer> squareIB;
		squareIB.reset(Aura::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 430
			layout(location = 0) in vec3 position;
			uniform mat4 u_ViewProjection;
			out vec3 v_Position;
			void main(){
				v_Position = position;
				gl_Position = u_ViewProjection * vec4(position, 1.0);
			}
		)";
		std::string fragmentSrc = R"(
			#version 430
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			void main(){
				color = vec4(v_Position*0.5+0.5,1.0);
			}
		)";

		m_Shader.reset(new Aura::Shader(vertexSrc, fragmentSrc));

	}

	void OnUpdate() override {
		if (Aura::Input::IsKeyPressed(AR_KEY_LEFT)) {
			m_CameraPosition.x -= m_CameraMoveSpeed;
		}
		else if (Aura::Input::IsKeyPressed(AR_KEY_RIGHT)) {
			m_CameraPosition.x += m_CameraMoveSpeed;
		}
		if (Aura::Input::IsKeyPressed(AR_KEY_UP)) {
			m_CameraPosition.y += m_CameraMoveSpeed;
		}
		else if (Aura::Input::IsKeyPressed(AR_KEY_DOWN)) {
			m_CameraPosition.y -= m_CameraMoveSpeed;
		}


		if (Aura::Input::IsKeyPressed(AR_KEY_A)) {
			m_CameraRotation += m_CameraRotationSpeed;
		}
		else if (Aura::Input::IsKeyPressed(AR_KEY_D)) {
			m_CameraRotation -= m_CameraRotationSpeed;
		}

		Aura::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Aura::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Aura::Renderer::BeginScene(m_Camera);

		Aura::Renderer::Submit(m_Shader, m_SquareVA);
		Aura::Renderer::Submit(m_Shader, m_VertexArray);

		Aura::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override {
		
	}

	void OnEvent(Aura::Event& event) override {
		Aura::EventDispatcher dispatcher(event);
		//dispatcher.Dispatch<Aura::KeyPressedEvent>(std::bind(&ExampleLayer::OnKeyPressed, this, std::placeholders::_1));
		dispatcher.Dispatch<Aura::KeyPressedEvent>(AR_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));
	}

	bool OnKeyPressedEvent(Aura::KeyPressedEvent& event) {
		//if (event.GetKeyCode() == Aura::Key::Escape) {
		//	AR_CORE_INFO("Escape key is pressed!");
		//}

		if (event.GetKeyCode() == AR_KEY_LEFT) {
			m_CameraPosition.x -= m_CameraMoveSpeed;
		}
		if (event.GetKeyCode() == AR_KEY_RIGHT) {
			m_CameraPosition.x += m_CameraMoveSpeed;
		}
		if (event.GetKeyCode() == AR_KEY_UP) {
			m_CameraPosition.y += m_CameraMoveSpeed;
		}
		if (event.GetKeyCode() == AR_KEY_DOWN) {
			m_CameraPosition.y -= m_CameraMoveSpeed;
		}

		return false;
	}
private:
	std::shared_ptr<Aura::Shader> m_Shader;
	std::shared_ptr<Aura::VertexArray> m_VertexArray;


	std::shared_ptr<Aura::VertexArray> m_SquareVA;

	Aura::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	
	float m_CameraMoveSpeed = 0.1f;
	
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 0.1f;
};

class Sandbox :public Aura::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		//PushOverlay(new Aura::ImGuiLayer());
	}
};

Aura::Application* Aura::CreateApplication() {
	return new Sandbox();
}