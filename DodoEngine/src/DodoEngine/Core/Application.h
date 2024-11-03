#pragma once

#include <DodoEngine/Core/Types.h>

#include <memory>

DODO_BEGIN_NAMESPACE

class Camera;
class EditorLayer;
class GameLayer;
class Renderer;
class Scene;
class Window;
struct WindowProps;

class Application
{
public:
  static Ref<Application> Create();

  void Init(const WindowProps &_windowProps);
  void AttachGameLayer(Ref<GameLayer> _gameLayer);

  void Run();

  Ref<Scene> GetCurrentScene() { return m_Scene; }

private:
  Ptr<Window> m_Window;
  Ptr<Renderer> m_Renderer;
  Ptr<Camera> m_Camera;
  Ptr<EditorLayer> m_EditorLayer;

  Ref<GameLayer> m_GameLayer;
  Ref<Scene> m_Scene;

  float m_LastFrameTime;
};

DODO_END_NAMESPACE