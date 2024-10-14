#include <DodoEngine/Core/Application.h>

#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>
#include <DodoEngine/Renderer/FbxLoader.h>
#include <DodoEngine/Renderer/GltfLoader.h>
#include <DodoEngine/Renderer/Mesh.h>
#include <DodoEngine/Renderer/MeshTransform.h>
#include <DodoEngine/Renderer/ObjLoader.h>
#include <DodoEngine/Renderer/Quad.h>
#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>

#include <functional>

#define _USE_MATH_DEFINES
#include <math.h>

DODO_BEGIN_NAMESPACE

Ptr<Application> Application::Create() {
  return std::make_unique<Application>();
}

void Application::Init(const WindowProps& _windowProps) {
  DODO_INFO("Initializing DodoEngine...");

  m_Window = std::make_unique<Window>();
  m_Window->Init(_windowProps);

  m_Renderer = std::make_unique<VulkanRenderer>(VulkanContext::Get());
  m_Renderer->Init(*m_Window);

  m_Camera = std::make_unique<Camera>(glm::vec3{0, 0, -2});

  m_ImGuiLayer = std::make_unique<ImGuiLayer>();
  m_ImGuiLayer->Init(*m_Window);
}

static float easeInOutSine(float x) {
  return -(cos(M_PI * x) - 1) / 2;
}

void Application::Run() {
  {
    float rotateDegree = 1;
    // Quad Vertex buffer
    std::vector<Ref<Mesh>> quadMeshes = {Mesh::Create(QUAD_VERTICES, QUAD_INDICES)};
    Ref<Model> quadModel = std::make_shared<Model>(quadMeshes);
    m_Renderer->RegisterModel(quadModel);

    std::vector<Ref<Mesh>> cubeMeshes = {Mesh::Create(CUBE_VERTICES, CUBE_INDICES)};
    Ref<Model> cubeMesh = std::make_shared<Model>(cubeMeshes);
    m_Renderer->RegisterModel(cubeMesh);

    // Ref<Mesh> hasbullah = FbxLoader::LoadFromFile("resources/models/Hasbullah.fbx");
    // Ref<Mesh> vikingRoom = ObjLoader::LoadFromFile("resources/models/viking/VikingRoom.obj", "resources/models/viking/viking_room.png");
    // Ref<Mesh> chestnut = ObjLoader::LoadFromFile("resources/models/chestnut/chestnut.obj", "resources/models/chestnut/model_Fire_emissive_transparent_color.tga.png");
    // Ref<Mesh> cube = ObjLoader::LoadFromFile("resources/models/cube/Cube.obj", "resources/models/cube/Cube.png");
    // Ref<Model> animatedCube = std::make_shared<Model>(ModelIdProvider::GetId(), GltfLoader::LoadFromFile("resources/models/animated-cube/AnimatedCube.gltf"));
    // Ref<Model> chessGame = std::make_shared<Model>(ModelIdProvider::GetId(), GltfLoader::LoadFromFile("resources/models/beautiful-game/ABeautifulGame.gltf"));
    // Ref<Model> tree = std::make_shared<Model>(ModelIdProvider::GetId(), GltfLoader::LoadFromFile("resources/models/tree/tree.gltf"));
    // Ref<Model> avocado = std::make_shared<Model>(GltfLoader::LoadFromFile("resources/models/avocado/Avocado.gltf"));
    // Ref<Model> sponza = std::make_shared<Model>(GltfLoader::LoadFromFile("resources/models/sponza/Sponza.gltf"));
    Ref<Model> sphere = std::make_shared<Model>(GltfLoader::LoadFromFile("resources/models/sphere/sphere.gltf"));
    const float movingSpeed = 0.1f;
    bool reverse = false;

    while (!m_Window->ShouldClose()) {
      DODO_TRACE(Application);

      m_Window->Update();

      const float time = m_Window->GetTime();
      float deltaTime = time - m_LastFrameTime;
      m_LastFrameTime = time;

      for (int i = 0; i < 10; ++i) {
        m_Renderer->DrawModel(sphere, {{i, 1, 1}, {0, 0, 0}, {0.5f, 0.5f, 0.5f}});
        m_Renderer->DrawModel(sphere, {{i, 1, -2}, {0, 0, 0}, {0.5f, 0.5f, 0.5f}});
      }
      
      const float pos = easeInOutSine(rotateDegree / 500) * 10.0f;
      const glm::vec3 lightPos = {pos, 2, 5.0f};
      m_Renderer->DrawCube({lightPos, {0, 0, 0}, {0.25f, 0.25f, 0.25f}});

      PerformanceManager::StoreFrameTime(deltaTime);

      if (rotateDegree == 0) {
        reverse = false;
      }

      if (rotateDegree == 500) {
        reverse = true;
      }

      if (reverse) {
        rotateDegree--;
      } else {
        rotateDegree++;
      }

      m_Camera->Update(deltaTime);

      // Main pass
      {
          m_Renderer->BeginRenderPass();
          m_Renderer->Update(*m_Camera, Light{lightPos}, deltaTime);
          m_Renderer->EndRenderPass();
      }

      // UI pass
      {
          m_Renderer->BeginUIRenderPass();
          m_ImGuiLayer->Update(m_Renderer->GetFrameIndex());
          m_Renderer->EndUIRenderPass();
      }


      m_Camera->Reset();
      m_Window->SwapBuffers();
    }
  }
  m_Window->Shutdown();
  m_ImGuiLayer->Shutdown();
  m_Renderer->Shutdown();
}

DODO_END_NAMESPACE
