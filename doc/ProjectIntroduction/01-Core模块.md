# Core 模块详解

## 1. 架构概述

Core 模块是 Aura Engine 的核心基础设施，提供应用程序生命周期管理、窗口管理、层叠系统、输入处理等核心功能。该模块采用**工厂模式**创建对象，通过**RAII**管理资源，并使用**单例模式**确保应用程序唯一性。

```
┌─────────────────────────────────────────────────────────────┐
│                        Application                          │
│     (单例模式 - 管理整个应用程序生命周期)                         │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              LayerStack (层栈)                       │    │
│  │  ┌──────────────────────────────────────────────┐   │    │
│  │  │ [Overlay] ← ImGuiLayer (最高优先级)            │   │    │
│  │  ├──────────────────────────────────────────────┤   │    │
│  │  │ [Layer] ← 用户自定义层                         │   │    │
│  │  ├──────────────────────────────────────────────┤   │    │
│  │  │ [Layer] ← Sandbox2D                          │   │    │
│  │  └──────────────────────────────────────────────┘   │    │
│  └─────────────────────────────────────────────────────┘    │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │   Window    │  │  ImGuiLayer │  │    Renderer         │  │
│  │ (unique_ptr)│  │             │  │    (静态初始化)       │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## 2. Application 类

### 2.1 类的职责

`Application` 是整个引擎的入口点，负责：
- 管理主循环 (Main Loop)
- 管理层栈 (LayerStack)
- 处理全局事件
- 管理窗口生命周期

### 2.2 核心代码解析

```cpp
class Application {
public:
    Application() {
        // 1. 确保单例唯一性
        AR_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        // 2. 创建窗口
        m_Window = std::unique_ptr<Window>(Window::Create());

        // 3. 设置事件回调
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        // 4. 初始化渲染器
        Renderer::Init();

        // 5. 创建 ImGui 层并压入栈顶
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    void Run() {
        while (m_Running) {
            // 计算帧时间
            float time = (float)glfwGetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized) {
                // 更新所有层
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(timestep);

                // 渲染 ImGui
                m_ImGuiLayer->Begin();
                for (Layer* layer : m_LayerStack)
                    layer->OnImGuiRender();
                m_ImGuiLayer->End();
            }

            // 交换缓冲区，轮询事件
            m_Window->OnUpdate();
        }
    }
};
```

### 2.3 主循环时序图

```
┌──────────────────────────────────────────────────────────────────┐
│                        主循环 (Main Loop)                         │
├──────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌──────────┐    ┌────────────┐    ┌───────────┐    ┌────────┐ │
│  │ 计算时间  │ -> │ 更新所有层  │ -> │ 渲染ImGui │ -> │ 交换缓冲│ │
│  │ timestep │    │ OnUpdate() │    │ OnImGui() │    │OnUpdate│ │
│  └──────────┘    └────────────┘    └───────────┘    └────────┘ │
│       │               │                 │                 │      │
│       v               v                 v                 v      │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │                     每帧执行一次                              │ │
│  └─────────────────────────────────────────────────────────────┘ │
│                              │                                    │
│                              │ (循环直到 m_Running = false)       │
│                              └─────────────────────────────────── │
└──────────────────────────────────────────────────────────────────┘
```

## 3. Window 抽象层

### 3.1 设计目的

Window 类提供了窗口操作的抽象接口，使得上层代码无需关心具体平台实现：

```cpp
class Window {
public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual void OnUpdate() = 0;
    virtual unsigned int GetWidth() const = 0;
    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
    virtual void SetVSync(bool enabled) = 0;

    static Window* Create(const WindowProps& props = WindowProps());
};
```

### 3.2 Windows 实现 (WindowsWindow)

```cpp
class WindowsWindow : public Window {
private:
    GLFWwindow* m_Window;           // 原生窗口句柄
    GraphicsContext* m_Context;      // OpenGL 上下文
    WindowData m_Data;                // 窗口数据

    struct WindowData {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;
        EventCallbackFn EventCallback;
    };
};
```

### 3.3 GLFW 回调设置

WindowsWindow 在初始化时设置 GLFW 事件回调：

```cpp
// 窗口大小改变回调
glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
    WindowResizeEvent event(width, height);
    data.EventCallback(event);
});

// 键盘事件回调
glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    // 根据 action 分发 KeyPressed 或 KeyReleased 事件
});

// 鼠标事件回调
glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
    // 分发鼠标按钮事件
});
```

## 4. Layer 与 LayerStack

### 4.1 Layer 类

Layer 是基本的层接口，所有游戏逻辑层都继承自此类：

```cpp
class Layer {
public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer();

    virtual void OnAttach() {}       // 层被添加到栈时调用
    virtual void OnDetach() {}        // 层从栈移除时调用
    virtual void OnUpdate(Timestep) {} // 每帧更新
    virtual void OnImGuiRender() {}   // ImGui 渲染
    virtual void OnEvent(Event&) {}   // 事件处理
};
```

### 4.2 LayerStack 容器

```cpp
class LayerSatck {
private:
    std::vector<Layer*> m_Layers;
    unsigned int m_LayerInsertIndex = 0;
};
```

### 4.3 层栈的组织结构

```
┌────────────────────────────────────────────────────────────────┐
│                        LayerStack 层次结构                       │
├────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌─────────────────────────────────────────────────────────┐   │
│   │  Overlays (后进先出 - LIFO)                              │   │
│   │  ┌─────────┐  ┌─────────┐  ┌─────────┐                  │   │
│   │  │ ImGui   │  │ Debug   │  │ Editor  │  ← PushOverlay() │   │
│   │  │ Layer   │  │ Panel   │  │ Panel   │                  │   │
│   │  └─────────┘  └─────────┘  └─────────┘                  │   │
│   ├─────────────────────────────────────────────────────────┤   │
│   │  Layers (先进先出 - FIFO)                                │   │
│   │  ┌─────────┐  ┌─────────┐  ┌─────────┐                  │   │
│   │  │ Sandbox │  │ Game    │  │ Physics │  ← PushLayer()    │   │
│   │  │ 2D      │  │ Layer   │  │ Layer   │                  │   │
│   │  └─────────┘  └─────────┘  └─────────┘                  │   │
│   └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│   PushLayer() 在 Overlay 之前插入，保证 Overlay 始终在顶层        │
│                                                                 │
└────────────────────────────────────────────────────────────────┘
```

### 4.4 事件传播顺序

当事件发生时，从 Overlay 到普通 Layer 依次传播：

```cpp
void Application::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

    // 反向迭代：从顶层到底层
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
        (*--it)->OnEvent(e);
        if (e.Handled)  // 如果事件已被处理，停止传播
            break;
    }
}
```

## 5. 智能指针封装

Core.h 中定义了两个重要的类型别名：

```cpp
namespace Aura {
    // 独占所有权 - 类似于 std::unique_ptr
    template<typename T>
    using Scope = std::unique_ptr<T>;

    // 共享所有权 - 类似于 std::shared_ptr
    template<typename T>
    using Ref = std::shared_ptr<T>;

    // 工厂函数
    template<typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}
```

### 使用示例

```cpp
// 创建 Shader（共享指针）
Ref<Shader> shader = Shader::Create("shader.glsl");

// 创建纹理（共享指针）
Ref<Texture2D> texture = Texture2D::Create("texture.png");

// 创建顶点缓冲区（独占指针）
Scope<VertexBuffer> vbo = VertexBuffer::Create(data, size);
```

## 6. 入口点模式

Aura Engine 使用 `main()` 函数作为程序入口，并通过 `CreateApplication()` 工厂函数允许用户自定义应用程序类：

```cpp
// EntryPoint.h
extern Aura::Application* Aura::CreateApplication();

int main(int argc, char** argv) {
    Aura::Log::Init();

    // 性能分析会话
    AR_PROFILE_BEGIN_SESSION("Startup", "AuraProfile-Startup.json");
    auto app = Aura::CreateApplication();
    AR_PROFILE_END_SESSION();

    AR_PROFILE_BEGIN_SESSION("Runtime", "AuraProfile-Runtime.json");
    app->Run();
    AR_PROFILE_END_SESSION();

    AR_PROFILE_BEGIN_SESSION("Shutdown", "AuraProfile-Shutdown.json");
    delete app;
    AR_PROFILE_END_SESSION();

    return 0;
}
```

用户只需实现 `CreateApplication()` 函数：

```cpp
// SandBoxApp.cpp
class SandBox : public Aura::Application {
public:
    SandBox() {
        PushLayer(new Sandbox2D());
    }
};

Aura::Application* Aura::CreateApplication() {
    return new SandBox();
}
```

## 7. 宏定义

Core.h 中定义了一系列实用宏：

```cpp
// DLL 导出/导入声明
#ifdef AR_PLATFORM_WINDOWS
    #define AURA_API __declspec(dllexport/dllimport)
#endif

// 断言宏
#define AR_ASSERT(x, ...) { if(!(x)) { AR_ERROR(...); __debugbreak(); } }
#define AR_CORE_ASSERT(x, ...) { /* 核心断言 */ }

// 位运算辅助 - 用于事件类别标志
#define BIT(x) (1 << x)

// 事件绑定宏
#define AR_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
```

## 8. 设计模式总结

| 模式 | 应用位置 | 说明 |
|------|---------|------|
| 单例模式 | Application::s_Instance | 确保唯一应用程序实例 |
| 工厂模式 | Window::Create(), Shader::Create() | 统一创建接口 |
| 观察者模式 | Event System | 事件发布-订阅 |
| 模板方法模式 | Layer | 定义更新/渲染骨架 |
| RAII | Scope/Ref 封装 | 自动资源管理 |
