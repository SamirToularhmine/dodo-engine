#pragma once

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Core/GameLayer.h>
#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Editor/EditorLayer.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Renderer/Renderer.h>

#include <memory>

DODO_BEGIN_NAMESPACE

class Application {
 public:
  static Ref<Application> Create();

  void Init(const WindowProps& _windowProps);
  void AttachGameLayer(Ref<GameLayer> _gameLayer);

  void Run();

  Ref<Scene>& GetCurrentScene() { return m_Scene; }

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