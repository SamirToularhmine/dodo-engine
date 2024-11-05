#include <DodoEngine/Core/Application.h>
#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Core/GameLayer.h>
#include <DodoEngine/Core/Window.h>
#include <DodoEngine/Editor/EditorLayer.h>
#include <DodoEngine/Editor/Scene.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>
#include <DodoEngine/Renderer/GltfLoader.h>
#include <DodoEngine/Renderer/Mesh.h>
#include <DodoEngine/Renderer/MeshTransform.h>
#include <DodoEngine/Renderer/ObjLoader.h>
#include <DodoEngine/Renderer/Quad.h>
#include <DodoEngine/Renderer/Renderer.h>
#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>

#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>

DODO_BEGIN_NAMESPACE

Ref<Application> Application::Create() { return MakeRef<Application>(); }

void Application::Init(const WindowProps &_windowProps)
{
  DODO_INFO("Initializing DodoEngine...");

  m_Window = MakePtr<Window>();
  m_Window->Init(_windowProps);

  m_Renderer = MakePtr<VulkanRenderer>(VulkanContext::Get());
  m_Renderer->Init(*m_Window);

  m_Camera = MakePtr<Camera>(glm::vec3{0, -2, -2});

  m_EditorLayer = MakePtr<EditorLayer>();
  m_EditorLayer->Init(*m_Window);

  m_Scene = MakeRef<Scene>();

  m_EditorLayer->LoadScene(m_Scene);
}

void Application::AttachGameLayer(Ref<GameLayer> _gameLayer)
{
  m_GameLayer = _gameLayer;
  m_GameLayer->Init();
}

void Application::Run()
{
  {
    uint32_t frameNumber = 0;
    const glm::vec3 lightPos = {5, 2, 5.0f};

    while (!m_Window->ShouldClose())
    {
      DODO_TRACE(Application);

      m_Window->Update();

      const float time = m_Window->GetTime();
      float deltaTime = time - m_LastFrameTime;
      m_LastFrameTime = time;

      PerformanceManager::StoreFrameTime(deltaTime);
      m_Camera->Update(deltaTime);

      Frame frame = m_Renderer->BeginFrame(frameNumber++);
      {
        // Scene pass
        {
          m_Renderer->Update(frame, *m_Scene, *m_Camera, Light{lightPos}, deltaTime);
        }

        // UI pass
        {
          m_EditorLayer->Update(frame, *m_Camera, *m_Renderer);
        }
      }
      m_Renderer->EndFrame(frame);

      m_Camera->Reset();
      m_Window->SwapBuffers();
    }
  }
  m_Window->Shutdown();
  m_EditorLayer->Shutdown();
  m_Scene->Shutdown();
  m_GameLayer->Shutdown();
  m_Renderer->Shutdown();
}

DODO_END_NAMESPACE
