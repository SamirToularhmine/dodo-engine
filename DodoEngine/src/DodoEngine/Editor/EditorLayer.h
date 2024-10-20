#pragma once

#include <DodoEngine/Core/Types.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

DODO_BEGIN_NAMESPACE

class Camera;
struct Frame;
class Renderer;
class Scene;
class Window;

class EditorLayer
{
public:
  EditorLayer() = default;

  ~EditorLayer() = default;

  void Init(const Window &_window);
  void Update(Frame &_frame, const Camera &_camera, Renderer &_renderer) const;
  void Shutdown() const;

  void LoadScene(Ref<Scene> &_scene);

private:
  void InitTheme();

private:
  Ref<Scene> m_Scene;
};

DODO_END_NAMESPACE