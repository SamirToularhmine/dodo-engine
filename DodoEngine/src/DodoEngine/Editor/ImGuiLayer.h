#pragma once

#include <DodoEngine/Core/Types.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

DODO_BEGIN_NAMESPACE

struct Frame;
class Renderer;
class Window;

class ImGuiLayer {
 public:
  ImGuiLayer() = default;

  ~ImGuiLayer() = default;

  void Init(const Window& _window) const;
  void Update(Frame& _frame, Renderer& _renderer) const;
  void Shutdown() const;
};

DODO_END_NAMESPACE