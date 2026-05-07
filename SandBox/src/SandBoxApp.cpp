#include <Aura.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> 
#include <glm/gtc/type_ptr.hpp> 

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

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
		m_Camera(-1.6f/0.9f, 1.6f/0.9f, -1.0f, 1.0f),
		m_CameraPosition(0.0f, 0.0f, 0.0f),
		m_SquarePosition(0.0f, 0.0f, 0.0f)
	{
		m_VertexArray.reset(Aura::VertexArray::Create());

		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		Aura::Ref<Aura::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer.reset(Aura::VertexBuffer::Create(vertices, sizeof(vertices)));
		Aura::BufferLayout layout = {
			{Aura::ShaderDataType::Float3, "position"}
		};
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);


		uint32_t indices[3] = { 0,1,2 };
		Aura::Ref<Aura::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer.reset(Aura::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
		//==================================square===========================
		m_SquareVA.reset(Aura::VertexArray::Create());
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,	1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,	1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,	0.0f, 1.0f
		};

		Aura::Ref<Aura::VertexBuffer> squareVB;
		squareVB.reset(Aura::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Aura::ShaderDataType::Float3, "position" },
			{ Aura::ShaderDataType::Float2, "texCoord" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0,1,2, 2,3,0 };
		Aura::Ref<Aura::IndexBuffer> squareIB;
		squareIB.reset(Aura::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 430
			layout(location = 0) in vec3 position;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;
			void main(){
				v_Position = position;
				gl_Position = u_ViewProjection * u_Transform * vec4(position, 1.0);
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
		m_Shader.reset(Aura::Shader::Create(vertexSrc, fragmentSrc));


		std::string squareVertexSrc = R"(
			#version 430
			layout(location = 0) in vec3 position;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec3 v_Position;
			void main(){
				v_Position = position;
				gl_Position = u_ViewProjection * u_Transform * vec4(position, 1.0);
			}
		)";
		std::string squareFragmentSrc = R"(
			#version 430
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			uniform vec3 u_Color;
			uniform vec3 u_LightDir;
			void main(){
				color = vec4(u_Color, 1.0);
			}
		)";
		m_SquareShader.reset(Aura::Shader::Create(squareVertexSrc, squareFragmentSrc));

		m_TextureShader.reset(Aura::Shader::Create("assets/shaders/Texture.glsl"));
		
		m_Texture = Aura::Texture2D::Create("assets/textures/Checkerboard.png");
		m_LogoTexture = Aura::Texture2D::Create("assets/textures/ChernoLogo.png");
		std::dynamic_pointer_cast<Aura::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<Aura::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);

	}

	void OnUpdate(Aura::Timestep timestep) override {
		if (Aura::Input::IsKeyPressed(AR_KEY_LEFT)) {
			m_CameraPosition.x -= m_CameraMoveSpeed * timestep;
		}
		else if (Aura::Input::IsKeyPressed(AR_KEY_RIGHT)) {
			m_CameraPosition.x += m_CameraMoveSpeed * timestep;
		}
		if (Aura::Input::IsKeyPressed(AR_KEY_UP)) {
			m_CameraPosition.y += m_CameraMoveSpeed * timestep;
		}
		else if (Aura::Input::IsKeyPressed(AR_KEY_DOWN)) {
			m_CameraPosition.y -= m_CameraMoveSpeed * timestep;
		}


		if (Aura::Input::IsKeyPressed(AR_KEY_A)) {
			m_CameraRotation += m_CameraRotationSpeed * timestep;
		}
		else if (Aura::Input::IsKeyPressed(AR_KEY_D)) {
			m_CameraRotation -= m_CameraRotationSpeed * timestep;
		}

		if (Aura::Input::IsKeyPressed(AR_KEY_J)) {
			m_SquarePosition.x -= m_SquareMoveSpeed * timestep;
		}
		else if (Aura::Input::IsKeyPressed(AR_KEY_L)) {
			m_SquarePosition.x += m_SquareMoveSpeed * timestep;
		}
		if (Aura::Input::IsKeyPressed(AR_KEY_I)) {
			m_SquarePosition.y += m_SquareMoveSpeed * timestep;
		}
		else if (Aura::Input::IsKeyPressed(AR_KEY_K)) {
			m_SquarePosition.y -= m_SquareMoveSpeed * timestep;
		}

		Aura::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Aura::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		//==================================draw===========================================
		Aura::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		//glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
		//glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

		//Aura::MaterialRef material = new Aura::Material(m_SquareShader);
		//Aura::MaterialInstanceRef mi = new Aura::MaterialInstance(material);

		//mi->Set("u_Color", redColor);
		//squareMesh->SetMaterial(mi);

		std::dynamic_pointer_cast<Aura::OpenGLShader>(m_SquareShader)->Bind();
		std::dynamic_pointer_cast<Aura::OpenGLShader>(m_SquareShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for(int y = 0; y < 20; y++){
			for (int x = 0; x < 20; x++) {
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

				
				Aura::Renderer::Submit(m_SquareShader, m_SquareVA, transform);
			}
		}

		m_Texture->Bind();
		Aura::Renderer::Submit(m_TextureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), m_SquarePosition));
		m_LogoTexture->Bind();
		Aura::Renderer::Submit(m_TextureShader, m_SquareVA, glm::translate(glm::mat4(1.0f), m_SquarePosition));
		//Trangle
		//Aura::Renderer::Submit(m_Shader, m_VertexArray);

		Aura::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override {
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Aura::Event& event) override {
		Aura::EventDispatcher dispatcher(event);
		//dispatcher.Dispatch<Aura::KeyPressedEvent>(AR_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));
	}

	bool OnKeyPressedEvent(Aura::KeyPressedEvent& event) {

	}
private:
	Aura::Ref<Aura::Shader> m_Shader;
	Aura::Ref<Aura::Shader> m_TextureShader;
	Aura::Ref<Aura::Shader> m_SquareShader;

	Aura::Ref<Aura::VertexArray> m_VertexArray;
	Aura::Ref<Aura::VertexArray> m_SquareVA;

	Aura::Ref<Aura::Texture2D> m_Texture;
	Aura::Ref<Aura::Texture2D> m_LogoTexture;

	Aura::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	
	float m_CameraMoveSpeed = 10.0f;
	
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec3 m_SquarePosition;
	float m_SquareMoveSpeed = 1.0f;

	glm::vec3 m_SquareColor{ 0.8f, 0.2f, 0.3f };
};

class Sandbox :public Aura::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}
};

Aura::Application* Aura::CreateApplication() {
	return new Sandbox();
}