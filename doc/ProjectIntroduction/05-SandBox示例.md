# SandBox 示例详解

## 1. SandBox 项目概述

SandBox 是 Aura Engine 的演示应用程序，展示了引擎的各项功能，包括：
- 2D 渲染
- 纹理贴图
- 相机控制
- ImGui 调试面板
- 着色器使用

```
┌─────────────────────────────────────────────────────────────────────┐
│                         SandBox 应用结构                            │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │                        SandBoxApp                             │  │
│  │  (入口点，创建 ExampleLayer 和 Sandbox2D)                       │  │
│  └───────────────────────────────────────────────────────────────┘  │
│                              │                                        │
│              ┌───────────────┼───────────────┐                      │
│              ▼               ▼               ▼                      │
│  ┌─────────────────┐ ┌─────────────┐ ┌─────────────────────┐        │
│  │   ExampleLayer  │ │ Sandbox2D   │ │  ImGuiLayer        │        │
│  │                 │ │             │ │  (内置)            │        │
│  │ - 三角形渲染     │ │ - 2D 渲染   │ │                    │        │
│  │ - 着色器使用    │ │ - 批量渲染  │ │  - Stats 面板      │        │
│  │ - 相机控制      │ │ - 相机控制  │ │  - Color 编辑器    │        │
│  └─────────────────┘ └─────────────┘ └─────────────────────┘        │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

## 2. 应用入口点

### 2.1 main() 函数

```cpp
// SandBoxApp.cpp
#include <Aura.h>
#include <Aura/Core/EntryPoint.h>

class ExampleLayer : public Aura::Layer { /* ... */ };
class Sandbox2D : public Aura::Layer { /* ... */ };

class SandBox : public Aura::Application {
public:
    SandBox() {
        PushLayer(new ExampleLayer());
        PushLayer(new Sandbox2D());  // SandBox2D 在 ExampleLayer 之上
    }
};

Aura::Application* Aura::CreateApplication() {
    return new SandBox();
}
```

### 2.2 层栈顺序

```cpp
PushLayer(new ExampleLayer());     // 先压入，在下面
PushLayer(new Sandbox2D());       // 后压入，在上面（覆盖 ExampleLayer）
PushOverlay(m_ImGuiLayer);        // 最后压入 overlay，始终在最顶层
```

渲染顺序（从下到上）：
1. ExampleLayer
2. Sandbox2D
3. ImGuiLayer (Overlay)

## 3. Sandbox2D 详解

### 3.1 类结构

```cpp
class Sandbox2D : public Aura::Layer {
private:
    Aura::OrthographicCameraController m_CameraController;

    // 渲染资源
    Aura::Ref<Aura::VertexArray> m_SquareVA;
    Aura::Ref<Aura::Shader> m_FlatColorShader;
    Aura::Ref<Aura::Texture2D> m_CheckerboardTexture;

    // 运行时数据
    glm::vec4 m_SquareColor{ 0.8f, 0.2f, 0.3f, 1.0f };
};
```

### 3.2 资源初始化 (OnAttach)

```cpp
void Sandbox2D::OnAttach() {
    AR_PROFILE_FUNCTION();

    // 加载纹理
    m_CheckerboardTexture = Aura::Texture2D::Create("assets/textures/Checkerboard.png");
}
```

### 3.3 每帧更新 (OnUpdate)

```cpp
void Sandbox2D::OnUpdate(Aura::Timestep timestep) {
    AR_PROFILE_FUNCTION();

    // 1. 更新相机（处理输入）
    {
        AR_PROFILE_SCOPE("Camera Update");
        m_CameraController.OnUpdate(timestep);
    }

    // 2. 重置统计并清屏
    Aura::Renderer2D::ResetStats();
    {
        AR_PROFILE_SCOPE("Render Prep");
        Aura::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Aura::RenderCommand::Clear();
    }

    // 3. 开始 2D 渲染场景
    {
        AR_PROFILE_SCOPE("Render Draw");

        // 开始场景，传入相机
        Aura::Renderer2D::BeginScene(m_CameraController.GetCamera());

        // 绘制各种四边形
        Aura::Renderer2D::DrawRotatedQuad({ -1.0f, 0.0f, 0.0f },
                                           { 1.0f, 1.0f },
                                           45.0f,
                                           { 1, 0, 1, 1 });

        Aura::Renderer2D::DrawQuad({ -1.0f, 0.0f },
                                    { 0.8f, 0.8f },
                                    { 0.8f, 0.2f, 0.3f, 1.0f });

        Aura::Renderer2D::DrawQuad({ 0.5f, -0.5f },
                                    { 0.5f, 0.75f },
                                    { 0.2f, 0.3f, 0.8f, 1.0f });

        // 绘制带纹理的四边形
        Aura::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f },
                                    { 10.0f, 10.0f },
                                    m_CheckerboardTexture,
                                    5.0f);  // tiling factor

        // 绘制旋转四边形
        Aura::Renderer2D::DrawRotatedQuad({ -1.0f, -1.5f, 1.0f },
                                           { 1.0f, 1.0f },
                                           rotation,
                                           m_CheckerboardTexture,
                                           20.0f);

        // 批量绘制循环（演示批量渲染）
        for (float y = -5.0f; y <= pressValue; y += 0.5f) {
            for (float x = -5.0f; x <= pressValue; x += 0.5f) {
                glm::vec4 color = {
                    (x + 5.0f) / 10.0f,
                    0.4f,
                    (y + 5.0f) / 10.0f,
                    0.5f
                };
                Aura::Renderer2D::DrawQuad({ x, y },
                                           { 0.45f, 0.45f },
                                           color);
            }
        }

        // 结束场景，提交渲染
        Aura::Renderer2D::EndScene();
    }
}
```

### 3.4 ImGui 调试面板 (OnImGuiRender)

```cpp
void Sandbox2D::OnImGuiRender() {
    AR_PROFILE_FUNCTION();

    ImGui::Begin("Settings");

    // 颜色编辑器
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

    // 滑块控件
    ImGui::SliderFloat("Tiling Factor", &titlingFactor, 1.0f, 10.0f);
    ImGui::SliderFloat("Rotation", &rotation, -180.0f, 180.0f);
    ImGui::InputFloat("Press Value", &pressValue, 5.0f, 100000.0f);

    // 渲染统计
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", Aura::Renderer2D::GetStats().DrawCalls);
    ImGui::Text("Quads: %d", Aura::Renderer2D::GetStats().QuadCount);
    ImGui::Text("Vertices: %d", Aura::Renderer2D::GetStats().GetTotalVertexCount());
    ImGui::Text("Indices: %d", Aura::Renderer2D::GetStats().GetTotalIndexCount());

    ImGui::End();
}
```

### 3.5 事件处理 (OnEvent)

```cpp
void Sandbox2D::OnEvent(Aura::Event& event) {
    EventDispatcher dispatcher(event);

    // 将鼠标滚动事件传递给相机控制器
    dispatcher.Dispatch<MouseScrolledEvent>(
        BIND_EVENT_FN(Sandbox2D::OnMouseScrolled)
    );
}

bool Sandbox2D::OnMouseScrolled(MouseScrolledEvent& event) {
    m_CameraController.SetZoomLevel(
        m_CameraController.GetZoomLevel() - event.GetYOffset() * 0.1f
    );
    return true;
}
```

## 4. ExampleLayer 详解

### 4.1 顶点缓冲区设置

```cpp
ExampleLayer() : Layer("Example") {
    // 创建顶点数组
    m_VertexArray = Aura::VertexArray::Create();

    // 顶点数据（三角形）
    float vertices[3 * 3] = {
        -0.5f, -0.5f, 0.0f,  // 顶点 0
         0.5f, -0.5f, 0.0f,  // 顶点 1
         0.0f,  0.5f, 0.0f   // 顶点 2
    };

    // 创建顶点缓冲区
    m_VertexBuffer = Aura::VertexBuffer::Create(vertices, sizeof(vertices));

    // 设置缓冲区布局
    Aura::BufferLayout layout = {
        { Aura::ShaderDataType::Float3, "position" }
    };
    m_VertexBuffer->SetLayout(layout);
    m_VertexArray->AddVertexBuffer(m_VertexBuffer);

    // 创建索引缓冲区
    uint32_t indices[3] = { 0, 1, 2 };
    m_IndexBuffer = Aura::IndexBuffer::Create(indices, 3);
    m_VertexArray->SetIndexBuffer(m_IndexBuffer);
}
```

### 4.2 着色器创建

```cpp
// 方法 1：从源码字符串创建
std::string vertexSrc = R"(
    #version 430
    layout(location = 0) in vec3 position;
    uniform mat4 u_ViewProjection;
    uniform mat4 u_Transform;
    void main(){
        gl_Position = u_ViewProjection * u_Transform * vec4(position, 1.0);
    }
)";

std::string fragmentSrc = R"(
    #version 430
    layout(location = 0) out vec4 color;
    uniform vec3 u_Color;
    void main(){
        color = vec4(u_Color, 1.0);
    }
)";

m_SquareShader = Aura::Shader::Create("SquareShader",
                                       vertexSrc, fragmentSrc);

// 方法 2：从文件加载
auto m_TextureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");
```

## 5. 着色器文件格式

### 5.1 FlatColor.glsl

```glsl
// 顶点着色器
#type vertex
#version 430

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main() {
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

// 片段着色器
#type fragment
#version 430

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main() {
    color = u_Color;
}
```

### 5.2 Texture.glsl

```glsl
#type vertex
#version 430

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;

void main() {
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
}

#type fragment
#version 430

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main() {
    color = texture(u_Texture, v_TexCoord);
}
```

## 6. 渲染流程时序图

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      SandBox 渲染时序图                                  │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│  ┌──────────────────────────────────────────────────────────────────┐   │
│  │                         Application::Run()                        │   │
│  │                                                                    │   │
│  │   ┌──────────────────────────────────────────────────────────┐    │   │
│  │   │  每帧:                                                     │    │   │
│  │   │                                                            │    │   │
│  │   │  1. 计算 Timestep                                          │    │   │
│  │   │       └─→ deltaTime                                        │    │   │
│  │   │                                                            │    │   │
│  │   │  2. Layer::OnUpdate(deltaTime)                            │    │   │
│  │   │       └─→ Sandbox2D::OnUpdate()                          │    │   │
│  │   │            ├─→ CameraController::OnUpdate()             │    │   │
│  │   │            │   └─→ 处理输入，更新相机矩阵                 │    │   │
│  │   │            │                                              │    │   │
│  │   │            ├─→ Renderer2D::BeginScene(camera)            │    │   │
│  │   │            │   └─→ 绑定 shader，设置 VP 矩阵              │    │   │
│  │   │            │                                              │    │   │
│  │   │            ├─→ Renderer2D::DrawQuad() × N               │    │   │
│  │   │            │   └─→ 填充顶点缓冲区                         │    │   │
│  │   │            │                                              │    │   │
│  │   │            └─→ Renderer2D::EndScene()                    │    │   │
│  │   │                └─→ Flush() → glDrawElements()            │    │   │
│  │   │                                                            │    │   │
│  │   │  3. ImGuiLayer::Begin()                                   │    │   │
│  │   │  4. Layer::OnImGuiRender()                                │    │   │
│  │   │       └─→ Sandbox2D::OnImGuiRender()                       │    │   │
│  │   │            └─→ ImGui::Begin("Settings")...                │    │   │
│  │   │  5. ImGuiLayer::End()                                     │    │   │
│  │   │                                                            │    │   │
│  │   │  6. Window::OnUpdate()                                    │    │   │
│  │   │       └─→ glfwSwapBuffers()                              │    │   │
│  │   │                                                            │    │   │
│  │   └──────────────────────────────────────────────────────────┘    │   │
│  └──────────────────────────────────────────────────────────────────┘   │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

## 7. 相机控制详解

### 7.1 相机移动

通过键盘输入控制相机位置：

```cpp
void OrthographicCameraController::OnUpdate(Timestep ts) {
    float speed = m_CameraTranslationSpeed * ts.GetSeconds();

    if (Aura::Input::IsKeyPressed(AR_KEY_A))
        m_CameraPosition.x -= speed;
    if (Aura::Input::IsKeyPressed(AR_KEY_D))
        m_CameraPosition.x += speed;
    if (Aura::Input::IsKeyPressed(AR_KEY_W))
        m_CameraPosition.y += speed;
    if (Aura::Input::IsKeyPressed(AR_KEY_S))
        m_CameraPosition.y -= speed;

    m_Camera.SetPosition(m_CameraPosition);
}
```

### 7.2 鼠标滚轮缩放

```cpp
bool OrthographicCameraController::OnMouseScrolled(
        MouseScrolledEvent& event) {

    m_ZoomLevel -= event.GetYOffset() * 0.1f;
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);  // 最小缩放
    m_ZoomLevel = std::min(m_ZoomLevel, 10.0f);  // 最大缩放

    float aspectRatio = m_AspectRatio;
    float left = -aspectRatio * m_ZoomLevel;
    float right = aspectRatio * m_ZoomLevel;
    float bottom = -m_ZoomLevel;
    float top = m_ZoomLevel;

    m_Camera.SetProjection(left, right, bottom, top);
    return false;
}
```

## 8. 性能分析

### 8.1 AR_PROFILE_SCOPE 宏

```cpp
void Sandbox2D::OnUpdate(Aura::Timestep timestep) {
    AR_PROFILE_FUNCTION();  // 函数级别分析

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
        Aura::Renderer2D::BeginScene(m_CameraController.GetCamera());
        // ...
    }
}
```

### 8.2 输出格式

分析结果输出为 Chrome Tracing JSON 格式：

```json
{
    "otherData": {},
    "traceEvents": [
        {},
        {"cat": "function", "dur": 1250, "name": "Camera Update", "ph": "X", "pid": 0, "tid": 1234, "ts": 100000},
        {"cat": "function", "dur": 8300, "name": "Render Draw", "ph": "X", "pid": 0, "tid": 1234, "ts": 101250}
    ]
}
```

## 9. 快速实验建议

1. **修改四边形颜色**：在 OnImGuiRender 中拖动 ColorEdit4 滑块

2. **调整纹理平铺**：修改 Tiling Factor 值

3. **旋转四边形**：修改 Rotation 值或拖动滑块

4. **添加新的 DrawQuad**：在循环中添加新的绘制调用

5. **修改相机速度**：在 OrthographicCameraController 中调整 m_CameraTranslationSpeed

6. **实验批量渲染**：增加循环次数观察 DrawCalls 统计变化
