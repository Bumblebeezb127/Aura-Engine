# Aura Engine

## 概述

Aura Engine 是一个用 C++ 编写的轻量级 2D 游戏引擎，专为学习游戏引擎架构和设计模式而开发。该引擎参考了 Cherno 的 Hazel 引擎架构，提供了一个清晰的模块化系统，帮助开发者理解现代游戏引擎的核心组件。

## 技术栈

| 类别 | 技术/库 |
|------|---------|
| 编程语言 | C++ |
| 图形 API | OpenGL |
| 窗口管理 | GLFW |
| 数学库 | GLM |
| 日志系统 | spdlog |
| UI 工具包 | ImGui |
| 性能分析 | 自定义 Instrumentation 系统 |

## 项目结构

```
AuraEngine/
├── Aura/                          # 引擎核心代码
│   ├── src/
│   │   ├── Aura/                  # 主引擎命名空间
│   │   │   ├── Core/              # 核心模块（Application, Window, Layer等）
│   │   │   ├── Events/            # 事件系统
│   │   │   ├── Renderer/          # 渲染系统
│   │   │   ├── ImGui/             # ImGui 集成
│   │   │   └── Debug/             # 调试工具
│   │   ├── Platform/              # 平台特定实现
│   │   │   ├── Windows/           # Windows 平台
│   │   │   └── OpenGL/            # OpenGL 实现
│   │   └── Aura.h                 # 主头文件
│   └── vendor/                    # 第三方库
│       ├── GLFW/                  # 窗口管理
│       ├── GLAD/                  # OpenGL 加载器
│       └── glm/                   # 数学库
├── SandBox/                       # 演示应用程序
│   ├── assets/                   # 资源文件（着色器、纹理）
│   └── src/                      # 示例代码
└── doc/                          # 文档
    └── ProjectIntroduction/       # 技术学习文档
```

## 核心模块

### 1. Core 模块
- **Application**: 应用程序主类，管理主循环和层栈
- **Window**: 窗口抽象层，支持不同平台
- **Layer/LayerStack**: 层管理系统，支持 overlay 和普通 layer
- **Input**: 输入处理抽象
- **Timestep**: 时间管理

### 2. Events 模块
- 事件驱动架构
- 支持 Window、Keyboard、Mouse 事件
- EventDispatcher 模式实现事件分发

### 3. Renderer 模块
- **Renderer**: 主渲染器，管理渲染流程
- **Renderer2D**: 2D 渲染器，支持批量渲染
- **RendererAPI**: 渲染 API 抽象（当前支持 OpenGL）
- **Shader/Buffer/Texture/VertexArray**: 图形资源管理

### 4. ImGui 模块
- 集成 ImGui 用于编辑器 UI
- ImGuiLayer 提供调试和信息展示功能

### 5. Debug 模块
- **Instrumentor**: 性能分析工具，输出 Chrome Tracing 格式

## 架构设计模式

### 1. 工厂模式 (Factory Pattern)
```cpp
Window::Create()           // 创建窗口实例
Shader::Create()           // 创建着色器
VertexBuffer::Create()     // 创建顶点缓冲区
```

### 2. 策略模式 (Strategy Pattern)
```cpp
RendererAPI::API           // 可切换的渲染后端
// 当前实现: OpenGLRendererAPI
```

### 3. 观察者模式 (Observer Pattern)
- 事件系统采用发布-订阅模式
- EventDispatcher 处理事件分发

### 4. 层叠模式 (Layer Pattern)
- LayerStack 支持层的叠加和覆盖
- Overlay 始终在最顶层渲染

### 5. RAII 模式
- Scope<T> = std::unique_ptr<T>
- Ref<T> = std::shared_ptr<T>

## 快速开始

### 编译项目
项目使用 Visual Studio 解决方案 (AuraEngine.sln)

### 创建您的第一个应用

```cpp
#include <Aura.h>

class MyGame : public Aura::Application
{
public:
    MyGame()
    {
        PushLayer(new MyGameLayer());
    }
};

Aura::Application* Aura::CreateApplication()
{
    return new MyGame();
}
```

### 运行示例

参考 [SandBox](./SandBox/src/SandBoxApp.cpp) 项目学习引擎使用方法。

## 文档导航

详细的模块技术文档请参阅 [ProjectIntroduction](./ProjectIntroduction/) 目录：

| 文档 | 内容 |
|------|------|
| [01-Core模块.md](./ProjectIntroduction/01-Core模块.md) | Application、Window、Layer 等核心类详解 |
| [02-Events模块.md](./ProjectIntroduction/02-Events模块.md) | 事件系统架构和实现 |
| [03-Renderer模块.md](./ProjectIntroduction/03-Renderer模块.md) | 渲染系统架构 |
| [04-Platform层.md](./ProjectIntroduction/04-Platform层.md) | 平台抽象和 OpenGL 实现 |
| [05-SandBox示例.md](./ProjectIntroduction/05-SandBox示例.md) | 引擎使用示例解析 |

## 许可证

本项目仅用于学习目的。
