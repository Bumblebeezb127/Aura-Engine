# Platform 层详解

## 1. 平台抽象架构

Aura Engine 采用**平台抽象层**设计，将底层平台相关的代码与引擎核心解耦。这种设计允许同一套引擎代码运行在不同的操作系统上（当前主要支持 Windows）。

```
┌─────────────────────────────────────────────────────────────────────┐
│                        平台抽象架构图                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│                          Aura Core                                   │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │  Window.h    Input.h    GraphicsContext.h                     │  │
│  │  (抽象接口)    (抽象接口)   (抽象接口)                           │  │
│  └───────────────────────────────────────────────────────────────┘  │
│                              │                                        │
│              ┌───────────────┼───────────────┐                      │
│              ▼               ▼               ▼                      │
│  ┌─────────────────┐ ┌─────────────┐ ┌─────────────────────┐        │
│  │   Windows       │ │    Linux    │ │       macOS         │        │
│  │ WindowsWindow   │ │ LinuxWindow │ │   CocoaWindow       │        │
│  │ WindowsInput    │ │ LinuxInput  │ │   CocoaInput        │        │
│  │ OpenGLContext   │ │ OpenGLCtx   │ │   MetalContext      │        │
│  └─────────────────┘ └─────────────┘ └─────────────────────┘        │
│                                                                      │
│                          GLFW + OpenGL                               │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

## 2. Window 平台实现

### 2.1 WindowsWindow 类结构

```cpp
class WindowsWindow : public Window {
private:
    GLFWwindow* m_Window;           // GLFW 窗口句柄
    GraphicsContext* m_Context;     // OpenGL 图形上下文
    WindowData m_Data;              // 窗口数据

    struct WindowData {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;
        EventCallbackFn EventCallback;
    };
};
```

### 2.2 窗口初始化流程

```cpp
void WindowsWindow::Init(const WindowProps& props) {
    // 1. 保存窗口属性
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    // 2. 初始化 GLFW（仅一次）
    if (!s_GLFWInitialized) {
        glfwInit();
        glfwSetErrorCallback(GLFWErrorCallback);
        s_GLFWInitialized = true;
    }

    // 3. 创建 GLFW 窗口
    m_Window = glfwCreateWindow(
        props.Width, props.Height,
        m_Data.Title.c_str(),
        nullptr, nullptr
    );

    // 4. 创建 OpenGL 上下文
    m_Context = new OpenGLContext(m_Window);
    m_Context->Init();

    // 5. 设置用户指针（用于回调中访问窗口数据）
    glfwSetWindowUserPointer(m_Window, &m_Data);

    // 6. 设置 VSync
    SetVSync(true);

    // 7. 设置 GLFW 事件回调
    Setup GLFWCallbacks();
}
```

### 2.3 GLFW 回调设置

```cpp
void WindowsWindow::Setup GLFWCallbacks() {
    // 窗口大小改变
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;
        WindowResizeEvent event(width, height);
        data.EventCallback(event);
    });

    // 窗口关闭
    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        WindowCloseEvent event;
        data.EventCallback(event);
    });

    // 键盘事件
    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event(key, 0);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event(key);
                data.EventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event(key, 1);
                data.EventCallback(event);
                break;
            }
        }
    });

    // 字符输入
    glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        KeyTypedEvent event(keycode);
        data.EventCallback(event);
    });

    // 鼠标按钮
    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event(button);
                data.EventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event(button);
                data.EventCallback(event);
                break;
            }
        }
    });

    // 鼠标滚动
    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseScrolledEvent event((float)xOffset, (float)yOffset);
        data.EventCallback(event);
    });

    // 鼠标移动
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseMovedEvent event((float)xPos, (float)yPos);
        data.EventCallback(event);
    });
}
```

## 3. GraphicsContext 图形上下文

### 3.1 抽象接口

```cpp
class GraphicsContext {
public:
    virtual void Init() = 0;
    virtual void SwapBuffers() = 0;
    virtual void* GetNativeContext() = 0;

    static Scope<GraphicsContext> Create(void* window);
};
```

### 3.2 OpenGL 上下文实现

```cpp
class OpenGLContext : public GraphicsContext {
private:
    GLFWwindow* m_Window;

public:
    OpenGLContext(GLFWwindow* window)
        : m_Window(window) {}

    virtual void Init() override {
        glfwMakeContextCurrent(m_Window);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        AR_CORE_ASSERT(status, "Failed to initialize GLAD!");
    }

    virtual void SwapBuffers() override {
        glfwSwapBuffers(m_Window);
    }
};
```

## 4. Input 平台实现

### 4.1 Input 抽象接口

```cpp
class Input {
protected:
    static Input* s_Instance;

public:
    inline static bool IsKeyPressed(int keycode) {
        return s_Instance->IsKeyPressedImpl(keycode);
    }

    inline static bool IsMouseButtonPressed(int button) {
        return s_Instance->IsMouseButtonPressedImpl(button);
    }

    inline static std::pair<float, float> GetMousePosition() {
        return s_Instance->GetMousePositionImpl();
    }

protected:
    virtual bool IsKeyPressedImpl(int keycode) = 0;
    virtual bool IsMouseButtonPressedImpl(int button) = 0;
    virtual std::pair<float, float> GetMousePositionImpl() = 0;
};
```

### 4.2 Windows Input 实现

```cpp
class WindowsInput : public Input {
protected:
    virtual bool IsKeyPressedImpl(int keycode) override {
        auto window = static_cast<GLFWwindow*>(
            Application::Get().GetWindow().GetNativeWindow()
        );
        return glfwGetKey(window, keycode) == GLFW_PRESS;
    }

    virtual bool IsMouseButtonPressedImpl(int button) override {
        auto window = static_cast<GLFWwindow*>(
            Application::Get().GetWindow().GetNativeWindow()
        );
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    virtual std::pair<float, float> GetMousePositionImpl() override {
        auto window = static_cast<GLFWwindow*>(
            Application::Get().GetWindow().GetNativeWindow()
        );
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return { (float)x, (float)y };
    }
};
```

## 5. OpenGL 渲染实现

### 5.1 OpenGLRendererAPI

```cpp
class OpenGLRendererAPI : public RendererAPI {
public:
    virtual void Init() override {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    virtual void SetClearColor(const glm::vec4& color) override {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    virtual void Clear() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    virtual void SetViewport(uint32_t x, uint32_t y,
                             uint32_t width, uint32_t height) override {
        glViewport(x, y, width, height);
    }

    virtual void DrawIndexed(const Ref<VertexArray>& vertexArray,
                             uint32_t indexCount) override {
        glDrawElements(GL_TRIANGLES,
                       indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount(),
                       GL_UNSIGNED_INT,
                       nullptr);
    }
};
```

### 5.2 OpenGLBuffer 实现

#### VertexBuffer

```cpp
class OpenGLVertexBuffer : public VertexBuffer {
private:
    uint32_t m_RendererID;
    BufferLayout m_Layout;

public:
    OpenGLVertexBuffer(float* vertices, uint32_t size) {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    virtual void Bind() const override {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    virtual const BufferLayout& GetLayout() const override {
        return m_Layout;
    }

    virtual void SetLayout(const BufferLayout& layout) override {
        m_Layout = layout;
    }
};
```

#### IndexBuffer

```cpp
class OpenGLIndexBuffer : public IndexBuffer {
private:
    uint32_t m_RendererID;
    uint32_t m_Count;

public:
    OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
        : m_Count(count) {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t),
                     indices, GL_STATIC_DRAW);
    }

    virtual void Bind() const override {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    virtual uint32_t GetCount() const override {
        return m_Count;
    }
};
```

### 5.3 OpenGLVertexArray 实现

```cpp
class OpenGLVertexArray : public VertexArray {
private:
    uint32_t m_RendererID;
    std::vector<Ref<VertexBuffer>> m_VertexBuffers;
    Ref<IndexBuffer> m_IndexBuffer;

public:
    OpenGLVertexArray() {
        glGenVertexArrays(1, &m_RendererID);
        glBindVertexArray(m_RendererID);
    }

    virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override {
        glBindVertexArray(m_RendererID);
        vertexBuffer->Bind();

        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            switch (element.Type) {
                case ShaderDataType::Float3: {
                    glEnableVertexAttribArray(location);
                    glVertexAttribPointer(location, 3, GL_FLOAT,
                                          element.Normalized ? GL_TRUE : GL_FALSE,
                                          layout.GetStride(),
                                          (const void*)element.Offset);
                    break;
                }
            }
            location++;
        }
        m_VertexBuffers.push_back(vertexBuffer);
    }

    virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override {
        glBindVertexArray(m_RendererID);
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }
};
```

### 5.4 OpenGLTexture 实现

```cpp
class OpenGLTexture2D : public Texture2D {
private:
    uint32_t m_RendererID;
    std::string m_Path;
    uint32_t m_Width, m_Height;

public:
    OpenGLTexture2D(const std::string& path);
    OpenGLTexture2D(uint32_t width, uint32_t height);

    virtual void Bind(uint32_t slot = 0) const override {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    virtual void SetData(void* data, uint32_t size) override {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                     m_Width, m_Height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    virtual uint32_t GetWidth() const override { return m_Width; }
    virtual uint32_t GetHeight() const override { return m_Height; }
};
```

## 6. OpenGL Shader 实现细节

### 6.1 GLSL 源码解析

```cpp
std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(
        const std::string& source) {

    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);

    while (pos != std::string::npos) {
        size_t eol = source.find_first_of("\r\n", pos);
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);

        shaderSources[ShaderTypeFromString(type)] =
            source.substr(nextLinePos,
                         pos - (nextLinePos == std::string::npos
                                ? source.size() - 1 : nextLinePos));
    }

    return shaderSources;
}
```

### 6.2 着色器编译

```cpp
void OpenGLShader::Compile(
        const std::unordered_map<GLenum, std::string>& shaderSources) {

    GLuint program = glCreateProgram();

    std::vector<GLuint> glShaders;
    for (auto& [type, source] : shaderSources) {
        GLuint shader = glCreateShader(ShaderTypeToGL(type));

        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        // 检查编译错误
        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (!isCompiled) {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<char> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
            AR_CORE_ERROR("Shader compile error: {0}", infoLog.data());
        }

        glAttachShader(program, shader);
        glShaders.push_back(shader);
    }

    glLinkProgram(program);

    // 检查链接错误
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (!isLinked) {
        // 处理链接错误
    }

    // 删除中间着色器对象
    for (auto shader : glShaders) {
        glDeleteShader(shader);
    }

    m_RendererID = program;
}
```

## 7. 外部库依赖

### 7.1 GLAD - OpenGL 加载器

GLAD 是一个开源的 OpenGL 加载器，用于在运行时动态加载 OpenGL 函数指针。

```
┌─────────────────────────────────────────────────────────────┐
│                      GLAD 工作原理                           │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│   ┌──────────────┐                                          │
│   │   你的代码    │                                          │
│   │ glDrawElements(...) │                                   │
│   └──────┬───────┘                                          │
│          │                                                   │
│          ▼                                                   │
│   ┌──────────────┐                                          │
│   │    GLAD      │  ← 统一接口                               │
│   │  (函数指针)   │                                          │
│   └──────┬───────┘                                          │
│          │                                                   │
│          ▼                                                   │
│   ┌──────────────┐                                          │
│   │    GLFW      │  ← 提供 OpenGL 上下文和函数获取地址       │
│   └──────────────┘                                          │
│          │                                                   │
│          ▼                                                   │
│   ┌──────────────┐                                          │
│   │   显卡驱动    │                                          │
│   └──────────────┘                                          │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 7.2 GLFW - 窗口管理

GLFW 是一个轻量级的 C 语言库，专门用于创建窗口和处理 OpenGL 上下文。

### 7.3 GLM - 数学库

GLM (OpenGL Mathematics) 是一个 header-only 的 C++ 数学库，提供：
- 向量类型：`glm::vec2`, `glm::vec3`, `glm::vec4`
- 矩阵类型：`glm::mat3`, `glm::mat4`
- 变换函数：`glm::translate`, `glm::rotate`, `glm::scale`
- 投影函数：`glm::ortho`, `glm::perspective`

## 8. 平台代码初始化顺序

```
┌─────────────────────────────────────────────────────────────────────┐
│                    引擎初始化顺序 (Windows)                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  1. Log::Init()                                                      │
│     └─→ 初始化 spdlog 日志系统                                        │
│                                                                      │
│  2. Window::Create()                                                 │
│     ├─→ glfwInit()                    (仅第一次)                      │
│     ├─→ glfwCreateWindow()                                          │
│     └─→ new OpenGLContext()                                         │
│         └─→ gladLoadGLLoader()                                       │
│                                                                      │
│  3. Renderer::Init()                                                 │
│     ├─→ RenderCommand::Init()                                        │
│     │   └─→ glEnable(GL_BLEND)                                       │
│     └─→ Renderer2D::Init()                                           │
│         ├─→ 创建 QuadVertexBuffer                                     │
│         ├─→ 创建 WhiteTexture                                         │
│         └─→ 编译 TextureShader                                        │
│                                                                      │
│  4. ImGuiLayer::OnAttach()                                           │
│     └─→ ImGui::CreateContext()                                       │
│     └─→ ImGui_ImplGlfw_Init()                                        │
│                                                                      │
│  5. Layer::OnAttach() (用户层)                                        │
│     └─→ 加载资源、初始化游戏逻辑                                       │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

## 9. 扩展到新平台

要将 Aura Engine 移植到新平台，需要实现以下抽象接口：

| 接口 | Windows 实现 | 新平台需要实现 |
|------|-------------|---------------|
| Window | WindowsWindow | X11Window, CocoaWindow |
| Input | WindowsInput | LinuxInput, CocoaInput |
| GraphicsContext | OpenGLContext | MetalContext, VulkanContext |
| VertexBuffer | OpenGLVertexBuffer | MetalVertexBuffer |
| IndexBuffer | OpenGLIndexBuffer | MetalIndexBuffer |
| VertexArray | OpenGLVertexArray | MetalVertexArray |
| Texture2D | OpenGLTexture2D | MetalTexture2D |
