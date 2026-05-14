# Events 模块详解

## 1. 事件系统架构

Aura Engine 采用**观察者模式**实现事件驱动架构。事件系统允许引擎各组件以松耦合方式通信，当事件发生时，事件从顶层 Layer 向下传播，直到被处理或到达栈底。

```
┌─────────────────────────────────────────────────────────────────────┐
│                         事件系统架构图                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   ┌──────────────┐         ┌──────────────────┐                      │
│   │   Window     │         │   Key/Mouse      │                      │
│   │   窗口事件    │         │   输入事件        │                      │
│   └──────┬───────┘         └────────┬─────────┘                      │
│          │                            │                               │
│          │ glfw callbacks             │                               │
│          ▼                            ▼                               │
│   ┌──────────────────────────────────────────────┐                   │
│   │              EventDispatcher                 │                   │
│   │  ┌─────────────────────────────────────────┐ │                   │
│   │  │ 事件分发器 - 根据事件类型路由到处理器         │ │                   │
│   │  └─────────────────────────────────────────┘ │                   │
│   └──────────────────────┬───────────────────────┘                    │
│                          │                                            │
│                          ▼                                            │
│   ┌──────────────────────────────────────────────┐                   │
│   │              LayerStack 传播                  │                   │
│   │  ┌────────┐  ┌────────┐  ┌────────┐          │                   │
│   │  │ImGui   │→ │Sandbox │→ │ Game   │→ ...     │                   │
│   │  │Layer   │  │2D      │  │ Layer  │          │                   │
│   │  └────────┘  └────────┘  └────────┘          │                   │
│   │     如果 e.Handled = true，停止传播            │                   │
│   └──────────────────────────────────────────────┘                   │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

## 2. Event 基类

### 2.1 事件类型枚举

```cpp
enum class EventType {
    None = 0,
    // 窗口事件
    WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
    // 应用程序事件
    AppTick, AppUpdate, AppRender,
    // 键盘事件
    KeyPressed, KeyReleased, KeyTyped,
    // 鼠标事件
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
};
```

### 2.2 事件类别标志 (Category Flags)

使用位标志表示事件类别，便于过滤：

```cpp
enum EventCategory {
    None = 0,
    EventCategoryApplication = BIT(0),   // 00001
    EventCategoryInput       = BIT(1),   // 00010
    EventCategoryKeyboard    = BIT(2),   // 00100
    EventCategoryMouse       = BIT(3),   // 01000
    EventCategoryMouseButton = BIT(4)    // 10000
};
```

### 2.3 Event 基类实现

```cpp
class Event {
    friend class EventDispatcher;

public:
    bool Handled = false;  // 事件是否已被处理

    virtual EventType GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;

    // 检查事件是否属于某类别
    inline bool IsInCategory(EventCategory category) {
        return GetCategoryFlags() & category;
    }
};
```

## 3. EventDispatcher 分发器

### 3.1 核心机制

EventDispatcher 使用**模板方法**和**std::function**实现类型安全的事件分发：

```cpp
class EventDispatcher {
    template<typename T>
    using EventFn = std::function<bool(T&)>;

public:
    EventDispatcher(Event& event)
        : m_Event(event) {}

    // 模板化分发函数
    template<typename T>
    bool Dispatch(EventFn<T> func) {
        // 运行时类型检查
        if (m_Event.GetEventType() == T::GetStaticType()) {
            // 调用处理函数
            m_Event.Handled = func(*(T*)&m_Event);
            return true;
        }
        return false;
    }

private:
    Event& m_Event;
};
```

### 3.2 使用示例

```cpp
void Application::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);

    // 分发窗口关闭事件
    dispatcher.Dispatch<WindowCloseEvent>(
        BIND_EVENT_FN(Application::OnWindowClose)
    );

    // 分发窗口大小调整事件
    dispatcher.Dispatch<WindowResizeEvent>(
        BIND_EVENT_FN(Application::OnWindowResize)
    );

    // 传播给各层
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
        (*--it)->OnEvent(e);
        if (e.Handled)
            break;
    }
}
```

## 4. 具体事件类型

### 4.1 窗口事件

```cpp
class WindowCloseEvent : public Event {
public:
    WindowCloseEvent() {}
    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class WindowResizeEvent : public Event {
public:
    WindowResizeEvent(unsigned int width, unsigned int height)
        : m_Width(width), m_Height(height) {}

    inline unsigned int GetWidth() const { return m_Width; }
    inline unsigned int GetHeight() const { return m_Height; }

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
    unsigned int m_Width, m_Height;
};
```

### 4.2 键盘事件

```
┌────────────────────────────────────────────────────────────┐
│                      键盘事件类型                            │
├────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌────────────────┐    ┌────────────────┐                    │
│  │ KeyPressed     │    │ KeyReleased    │                   │
│  │ 按下事件        │    │ 释放事件        │                   │
│  │ (含repeat计数) │    │                │                    │
│  └────────────────┘    └────────────────┘                    │
│                                                             │
│  ┌────────────────┐                                         │
│  │ KeyTyped       │                                         │
│  │ 字符输入事件    │ ← 用于文本输入，配合 ImGui 使用          │
│  └────────────────┘                                         │
│                                                             │
└────────────────────────────────────────────────────────────┘
```

```cpp
class KeyPressedEvent : public KeyEvent {
public:
    KeyPressedEvent(int keycode, int repeatCount)
        : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

    inline int GetRepeatCount() const { return m_RepeatCount; }

    EVENT_CLASS_TYPE(KeyPressed)
private:
    int m_RepeatCount;
};

class KeyTypedEvent : public KeyEvent {
public:
    KeyTypedEvent(int keycode)
        : KeyEvent(keycode) {}

    EVENT_CLASS_TYPE(KeyTyped)
};
```

### 4.3 鼠标事件

```cpp
class MouseMovedEvent : public Event {
public:
    MouseMovedEvent(float x, float y)
        : m_MouseX(x), m_MouseY(y) {}

    inline float GetX() const { return m_MouseX; }
    inline float GetY() const { return m_MouseY; }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    float m_MouseX, m_MouseY;
};

class MouseScrolledEvent : public Event {
public:
    MouseScrolledEvent(float xOffset, float yOffset)
        : m_XOffset(xOffset), m_YOffset(yOffset) {}

    inline float GetXOffset() const { return m_XOffset; }
    inline float GetYOffset() const { return m_YOffset; }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
};

class MouseButtonPressedEvent : public MouseButtonEvent {
public:
    MouseButtonPressedEvent(int button)
        : MouseButtonEvent(button) {}

    EVENT_CLASS_TYPE(MouseButtonPressed)
};
```

## 5. 宏定义解析

### EVENT_CLASS_TYPE 宏

```cpp
#define EVENT_CLASS_TYPE(type)                                                      \
    static EventType GetStaticType() { return EventType::type; }                  \
    virtual EventType GetEventType() const override { return GetStaticType(); }    \
    virtual const char* GetName() const override { return #type; }
```

展开后：

```cpp
class WindowCloseEvent : public Event {
public:
    static EventType GetStaticType() { return EventType::WindowClose; }
    virtual EventType GetEventType() const override { return GetStaticType(); }
    virtual const char* GetName() const override { return "WindowClose"; }
    // ...
};
```

### EVENT_CLASS_CATEGORY 宏

```cpp
#define EVENT_CLASS_CATEGORY(category) \
    virtual int GetCategoryFlags() const override { return category; }
```

## 6. 事件处理流程

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         事件处理完整流程                                  │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│  1. GLFW 原始事件                                                        │
│  ┌──────────────────────────────────────────────────────────────────┐   │
│  │ glfwSetKeyCallback(window, [](...){                              │   │
│  │     KeyPressedEvent event(key, repeatCount);                    │   │
│  │     data.EventCallback(event);  // 调用窗口的回调                │   │
│  │ });                                                              │   │
│  └──────────────────────────────────────────────────────────────────┘   │
│                                    │                                      │
│                                    ▼                                      │
│  2. 窗口事件回调                                                           │
│  ┌──────────────────────────────────────────────────────────────────┐   │
│  │ m_Window->SetEventCallback([this](Event& e){                    │   │
│  │     this->OnEvent(e);  // 传递给 Application                    │   │
│  │ });                                                              │   │
│  └──────────────────────────────────────────────────────────────────┘   │
│                                    │                                      │
│                                    ▼                                      │
│  3. Application 事件分发                                                    │
│  ┌──────────────────────────────────────────────────────────────────┐   │
│  │ EventDispatcher dispatcher(e);                                   │   │
│  │ dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(...));       │   │
│  │ dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(...));     │   │
│  └──────────────────────────────────────────────────────────────────┘   │
│                                    │                                      │
│                                    ▼                                      │
│  4. LayerStack 传播                                                         │
│  ┌──────────────────────────────────────────────────────────────────┐   │
│  │ for (Layer* layer : m_LayerStack) {                              │   │
│  │     layer->OnEvent(e);                                           │   │
│  │     if (e.Handled) break;  // 已处理则停止传播                     │   │
│  │ }                                                                │   │
│  └──────────────────────────────────────────────────────────────────┘   │
│                                    │                                      │
│                                    ▼                                      │
│  5. 用户层处理 (例如 Sandbox2D)                                              │
│  ┌──────────────────────────────────────────────────────────────────┐   │
│  │ void Sandbox2D::OnEvent(Event& event) {                         │   │
│  │     EventDispatcher dispatcher(event);                           │   │
│  │     dispatcher.Dispatch<MouseScrolledEvent>(                     │   │
│  │         BIND_EVENT_FN(Sandbox2D::OnMouseScrolled));              │   │
│  │ }                                                                │   │
│  └──────────────────────────────────────────────────────────────────┘   │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

## 7. 设计优势

| 特性 | 说明 |
|------|------|
| **类型安全** | 通过模板和虚函数实现编译时类型检查 |
| **松耦合** | 事件发送者无需知道接收者 |
| **可扩展** | 添加新事件类型只需继承 Event 基类 |
| **可中断** | Handled 标志允许提前停止传播 |
| **灵活过滤** | Category 标志允许按类型过滤事件 |

## 8. 实际使用示例

```cpp
// Sandbox2D.h 中处理相机缩放事件
class Sandbox2D : public Aura::Layer {
private:
    bool OnMouseScrolled(MouseScrolledEvent& event) {
        m_CameraController.SetZoomLevel(
            m_CameraController.GetZoomLevel() - event.GetYOffset() * 0.1f
        );
        return true;  // 标记为已处理
    }
};

void Sandbox2D::OnEvent(Aura::Event& event) {
    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<MouseScrolledEvent>(
        BIND_EVENT_FN(Sandbox2D::OnMouseScrolled)
    );
}
```
