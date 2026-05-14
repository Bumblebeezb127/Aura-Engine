# Renderer 模块详解

## 1. 渲染系统架构概述

Aura Engine 的渲染系统采用**策略模式**设计，通过抽象渲染 API 接口，支持不同的图形后端（当前实现为 OpenGL）。整体架构分为三层：

```
┌─────────────────────────────────────────────────────────────────────┐
│                        渲染系统分层架构                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                   Renderer (高层渲染器)                       │   │
│  │         管理场景数据、执行 BeginScene/EndScene                 │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                              │                                       │
│                              ▼                                       │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                  Renderer2D (2D 专用渲染器)                   │   │
│  │              批量渲染四边形、支持纹理和颜色                      │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                              │                                       │
│                              ▼                                       │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │              RenderCommand (渲染命令队列)                      │   │
│  │            静态分发接口，不存储状态                            │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                              │                                       │
│                              ▼                                       │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │            RendererAPI (抽象渲染 API)                         │   │
│  │         Init/SetViewport/SetClearColor/DrawIndexed           │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                              │                                       │
│                              ▼                                       │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │         OpenGLRendererAPI (OpenGL 具体实现)                   │   │
│  │              glViewport/glClearColor...                      │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

## 2. RendererAPI 抽象层

### 2.1 渲染 API 枚举

```cpp
class RendererAPI {
public:
    enum class API {
        None = 0,
        OpenGL = 1
    };
    // 获取当前使用的 API
    inline static API GetAPI() { return s_API; }
};
```

### 2.2 抽象接口定义

```cpp
class RendererAPI {
public:
    virtual void Init() = 0;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void SetClearColor(const glm::vec4& color) = 0;
    virtual void Clear() = 0;
    virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) = 0;
};
```

## 3. RenderCommand 静态封装

RenderCommand 是一个静态封装类，提供统一的渲染命令接口：

```cpp
class RenderCommand {
private:
    static RendererAPI* s_RendererAPI;  // 指向实际实现的指针

public:
    inline static void Init() {
        s_RendererAPI->Init();
    }

    inline static void DrawIndexed(const Ref<VertexArray>& vertexArray,
                                    uint32_t indexCount = 0) {
        s_RendererAPI->DrawIndexed(vertexArray, indexCount);
    }

    inline static void SetClearColor(const glm::vec4& color) {
        s_RendererAPI->SetClearColor(color);
    }

    inline static void Clear() {
        s_RendererAPI->Clear();
    }

    inline static void SetViewport(uint32_t x, uint32_t y,
                                    uint32_t width, uint32_t height) {
        s_RendererAPI->SetViewport(x, y, width, height);
    }
};
```

## 4. Shader 系统

### 4.1 Shader 抽象接口

```cpp
class Shader {
public:
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    // Uniform 设置接口
    virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
    virtual void SetFloat(const std::string& name, const float& value) = 0;
    virtual void SetInt(const std::string& name, const int& value) = 0;
    // ... 更多 Set 方法

    static Ref<Shader> Create(const std::string& filepath);
    static Ref<Shader> Create(const std::string& name,
                               const std::string& vertexSrc,
                               const std::string& fragmentSrc);
};
```

### 4.2 Shader 库管理

```cpp
class ShaderLibrary {
private:
    std::unordered_map<std::string, Ref<Shader>> m_Shaders;

public:
    void Add(const std::string& name, const Ref<Shader>& shader);
    Ref<Shader> Load(const std::string& filepath);
    Ref<Shader> Get(const std::string& name);
    bool Exists(const std::string& name) const;
};
```

### 4.3 OpenGL Shader 实现

OpenGLShader 负责加载、编译 GLSL 着色器：

```cpp
class OpenGLShader : public Shader {
private:
    uint32_t m_RendererID;    // OpenGL shader program ID
    std::string m_Name;       // 着色器名称

public:
    OpenGLShader(const std::string& filepath);
    OpenGLShader(const std::string& name,
                  const std::string& vertexSrc,
                  const std::string& fragmentSrc);

    void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

    void Bind() const override {
        glUseProgram(m_RendererID);
    }

    void SetMat4(const std::string& name, const glm::mat4& value) override {
        UploadUniformMat4(name, value);
    }

    void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }
};
```

## 5. Buffer 系统

### 5.1 Buffer 布局元素

```cpp
struct BufferElement {
    std::string Name;           // 变量名
    ShaderDataType Type;        // 数据类型
    uint32_t Size;             // 字节大小
    uint32_t Offset;           // 偏移量
    bool Normalized;            // 是否归一化

    uint32_t GetComponentCount() const {
        switch (Type) {
            case ShaderDataType::Float:  return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 3;
            case ShaderDataType::Float4: return 4;
            // ...
        }
    }
};
```

### 5.2 BufferLayout 自动计算

```cpp
class BufferLayout {
private:
    std::vector<BufferElement> m_Elements;
    uint32_t m_Stride = 0;

    void CalculateOffsetsAndStride() {
        uint32_t offset = 0;
        for (auto& element : m_Elements) {
            element.Offset = offset;
            offset += element.Size;
            m_Stride += element.Size;
        }
    }
};
```

### 5.3 顶点缓冲区和索引缓冲区

```cpp
class VertexBuffer {
public:
    virtual void Bind() const = 0;
    virtual void SetLayout(const BufferLayout& layout) = 0;
    virtual const BufferLayout& GetLayout() const = 0;
    virtual void SetData(const void* data, uint32_t size) = 0;

    static Ref<VertexBuffer> Create(uint32_t size);
    static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
};

class IndexBuffer {
public:
    virtual void Bind() const = 0;
    virtual uint32_t GetCount() const = 0;

    static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
};
```

## 6. VertexArray 系统

VertexArray 封装了 VAO (Vertex Array Object)：

```cpp
class VertexArray {
public:
    virtual void Bind() const = 0;
    virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
    virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

    virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
    virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

    static Ref<VertexArray> Create();
};
```

```
┌──────────────────────────────────────────────────────────────┐
│                    VertexArray 结构                           │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                    VertexArray (VAO)                     │ │
│  │                                                          │ │
│  │  ┌─────────────────────────────────────────────────────┐ │ │
│  │  │  VertexBuffer 0  ──→ BufferLayout                    │ │ │
│  │  │     position (Float3), texCoord (Float2)            │ │ │
│  │  ├─────────────────────────────────────────────────────┤ │ │
│  │  │  VertexBuffer 1  ──→ BufferLayout                   │ │ │
│  │  │     normal (Float3), tangent (Float3)               │ │ │
│  │  ├─────────────────────────────────────────────────────┤ │ │
│  │  │  IndexBuffer    ──→ 6 indices                       │ │ │
│  │  └─────────────────────────────────────────────────────┘ │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

## 7. Renderer2D 批量渲染系统

### 7.1 设计目标

Renderer2D 使用**批量渲染**技术，将多个 Draw Call 合并为少数几个，提高渲染效率。

### 7.2 核心数据结构

```cpp
struct Renderer2DData {
    static const uint32_t MaxQuads = 10;
    static const uint32_t MaxVertices = MaxQuads * 4;      // 40 顶点
    static const uint32_t MaxIndices = MaxQuads * 6;      // 60 索引
    static const uint32_t MaxTextureSlots = 32;

    Ref<VertexArray> QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;

    QuadVertex* QuadVertexBufferBase = nullptr;  // 顶点数据基指针
    QuadVertex* QuadVertexBufferPtr = nullptr;    // 当前写入位置

    uint32_t QuadIndexCount = 0;

    std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
    uint32_t TextureSlotIndex = 1;  // 0 = white texture

    Renderer2D::StaticCounters Stats;
};
```

### 7.3 QuadVertex 结构

每个四边形顶点包含 5 个属性：

```cpp
struct QuadVertex {
    glm::vec3  Position;     // 位置 (12 bytes)
    glm::vec4  Color;         // 颜色 (16 bytes)
    glm::vec2  TexCoord;     // 纹理坐标 (8 bytes)
    float      TexIndex;     // 纹理槽索引 (4 bytes)
    float      TilingFactor; // 纹理平铺因子 (4 bytes)
    // Total: 44 bytes per vertex
};
```

### 7.4 渲染流程

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Renderer2D 批量渲染流程                           │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  BeginScene(camera)                                                  │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │ 1. 绑定 TextureShader                                        │    │
│  │ 2. 设置 ViewProjection 矩阵                                  │    │
│  │ 3. 重置 QuadIndexCount = 0                                   │    │
│  │ 4. 重置 TextureSlotIndex = 1                                │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                              │                                       │
│                              ▼                                       │
│  DrawQuad(position, size, color)  × N 次                            │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │  if (QuadIndexCount >= MaxIndices)                          │    │
│  │      FlushAndReset();  // 批量提交当前顶点                   │    │
│  │                                                             │    │
│  │  // 填充 4 个顶点到顶点缓冲区                                 │    │
│  │  QuadVertexBufferPtr->Position = transform * VertexPos[0];   │    │
│  │  QuadVertexBufferPtr->Color = color;                        │    │
│  │  // ...                                                     │    │
│  │  QuadIndexCount += 6;  // 6 indices for one quad           │    │
│  │  QuadCount++;                                                │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                              │                                       │
│                              ▼                                       │
│  EndScene()                                                         │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │ 1. 上传顶点数据到 GPU                                        │    │
│  │ 2. 调用 Flush() 执行渲染                                    │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                                                                      │
│  Flush()                                                            │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │  for each texture slot:                                      │    │
│  │      texture->Bind(slot);                                   │    │
│  │                                                              │    │
│  │  glDrawElements(GL_TRIANGLES, QuadIndexCount,              │    │
│  │                   GL_UNSIGNED_INT, nullptr);               │    │
│  │                                                              │    │
│  │  Stats.DrawCalls++;                                          │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 7.5 纹理管理

Renderer2D 使用纹理槽数组管理纹理：

```cpp
// 查找或添加纹理槽
float Renderer2DData::GetTextureSlotIndex(Ref<Texture2D> texture) {
    for (uint32_t i = 0; i < TextureSlotIndex; i++) {
        if (*TextureSlots[i] == *texture) {
            return (float)i;
        }
    }
    // 新纹理
    TextureSlots[TextureSlotIndex] = texture;
    return (float)TextureSlotIndex++;
}
```

### 7.6 DrawQuad 实现

```cpp
void Renderer2D::DrawQuad(const glm::vec3& position,
                          const glm::vec2& size,
                          const glm::vec4& color) {
    if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
        FlushAndReset();
    }

    const float textureIndex = 0.0f;  // White texture
    const float tilingFactor = 1.0f;

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                         * glm::scale(glm::mat4(1.0f), { size, 1.0f });

    // 填充 4 个顶点
    for (int i = 0; i < 4; i++) {
        s_Data.QuadVertexBufferPtr->Position = transform * QuadVertexPositions[i];
        s_Data.QuadVertexBufferPtr->Color = color;
        s_Data.QuadVertexBufferPtr->TexCoord = TexCoords[i];
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;
    s_Data.Stats.QuadCount++;
}
```

## 8. OrthographicCamera 正交相机

### 8.1 相机投影

```cpp
class OrthographicCamera {
public:
    OrthographicCamera(float left, float right, float bottom, float top) {
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        RecalculateViewMatrix();
    }

    void SetPosition(const glm::vec3& position) {
        m_Position = position;
        RecalculateViewMatrix();
    }

    const glm::mat4& GetViewProjectionMatrix() const {
        return m_ViewProjectionMatrix;
    }

private:
    void RecalculateViewMatrix() {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)
                             * glm::rotate(glm::mat4(1.0f),
                                          glm::radians(m_Rotation),
                                          { 0, 0, 1 });

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ViewProjectionMatrix;
};
```

### 8.2 相机控制器

```cpp
class OrthographicCameraController {
public:
    OrthographicCameraController(float aspectRatio, bool rotation = false);

    void OnUpdate(Timestep ts);
    void OnEvent(Event& e);

    OrthographicCamera& GetCamera() { return m_Camera; }

    float GetZoomLevel() const { return m_ZoomLevel; }
    void SetZoomLevel(float level) { m_ZoomLevel = level; }

private:
    bool OnMouseScrolled(MouseScrolledEvent& event);
    bool OnWindowResized(WindowResizeEvent& event);

private:
    float m_AspectRatio;
    float m_ZoomLevel = 1.0f;
    OrthographicCamera m_Camera;
    bool m_Rotation;
};
```

## 9. 渲染统计

Renderer2D 提供运行时统计数据：

```cpp
struct StaticCounters {
    uint32_t DrawCalls = 0;   // 渲染调用次数
    uint32_t QuadCount = 0;   // 四边形数量

    uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
    uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
};

static StaticCounters& GetStats();
static void ResetStats();
```

## 10. 设计模式总结

| 模式 | 应用 | 说明 |
|------|------|------|
| 策略模式 | RendererAPI | 可切换渲染后端 |
| 工厂模式 | Shader/Buffer/Create | 统一创建接口 |
| 桥接模式 | Shader 抽象 | 着色器代码与硬件解耦 |
| 批量渲染 | Renderer2D | 合并 Draw Call |
| RAII | 所有图形资源 | 自动资源管理 |
