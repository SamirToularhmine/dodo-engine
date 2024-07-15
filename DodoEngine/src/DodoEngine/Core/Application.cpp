#include <DodoEngine/Core/Application.h>

#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>
#include <DodoEngine/Renderer/FbxLoader.h>
#include <DodoEngine/Renderer/Mesh.h>
#include <DodoEngine/Renderer/MeshTransform.h>
#include <DodoEngine/Renderer/Quad.h>
#include <DodoEngine/Renderer/GltfLoader.h>
#include <DodoEngine/Renderer/ObjLoader.h>
#include <DodoEngine/Utils/Log.h>
#include <DodoEngine/Utils/Utils.h>

#include <functional>

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

  m_Camera = std::make_unique<Camera>(glm::vec3{2, 2, 2});

  m_ImGuiLayer = std::make_unique<ImGuiLayer>();
  m_ImGuiLayer->Init(*m_Window);
}

void Application::Run() {
  {
    float rotateDegree = 0;
    // Quad Vertex buffer
    // Ref<Model> quadModel = std::make_shared<Model>(ModelIdProvider::GetId(), Mesh::Create({QUAD_VERTICES}, {QUAD_INDICES}));
    // m_Renderer->RegisterModel(quadModel);

    // Ref<Mesh> cubeMesh = Mesh::Create(CUBE_VERTICES, CUBE_INDICES);
    // m_Renderer->RegisterMesh(cubeMesh);

    // Ref<Mesh> hasbullah = FbxLoader::LoadFromFile("resources/models/Hasbullah.fbx");
    // Ref<Mesh> vikingRoom = ObjLoader::LoadFromFile("resources/models/viking/VikingRoom.obj", "resources/models/viking/viking_room.png");
    // Ref<Mesh> chestnut = ObjLoader::LoadFromFile("resources/models/chestnut/chestnut.obj", "resources/models/chestnut/model_Fire_emissive_transparent_color.tga.png");
    // Ref<Mesh> cube = ObjLoader::LoadFromFile("resources/models/cube/Cube.obj", "resources/models/cube/Cube.png");
    // Ref<Model> animatedCube = std::make_shared<Model>(ModelIdProvider::GetId(), GltfLoader::LoadFromFile("resources/models/animated-cube/AnimatedCube.gltf"));
    // Ref<Model> chessGame = std::make_shared<Model>(ModelIdProvider::GetId(), GltfLoader::LoadFromFile("resources/models/beautiful-game/ABeautifulGame.gltf"));
    // Ref<Model> tree = std::make_shared<Model>(ModelIdProvider::GetId(), GltfLoader::LoadFromFile("resources/models/tree/tree.gltf"));
    // Ref<Model> avocado = std::make_shared<Model>(ModelIdProvider::GetId(), GltfLoader::LoadFromFile("resources/models/avocado/Avocado.gltf"));
    Ref<Model> sponza = std::make_shared<Model>(ModelIdProvider::GetId(), GltfLoader::LoadFromFile("resources/models/sponza/Sponza.gltf"));

    while (!m_Window->ShouldClose()) {
      DODO_TRACE(Application);

      m_Window->Update();

      const float time = m_Window->GetTime();
      float deltaTime = time - m_LastFrameTime;
      m_LastFrameTime = time;

      std::cout << "\r" << deltaTime * 1000;

      for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
          for (int k = 0; k < 8; k++)
            // m_Renderer->DrawCube({{2 * i, 2 * j, 2 * k}, {rotateDegree, rotateDegree, rotateDegree}, {1, 1, 1}});

      PerformanceManager::StoreFrameTime(deltaTime);

      //m_Renderer->DrawCube({{0, 0, 0}, {0, 0, 0}, {5, 5, 5}});
      m_Renderer->DrawModel(sponza, {{0, 0, 0}, {0, 0, 0}, {1, 1, 1}});
      // m_Renderer->DrawQuad({{0, 0, 0}, {0, 0, 0}, {1, 1, 1}});

      rotateDegree++;
      rotateDegree = (int)rotateDegree % 360;

      m_Camera->Update(deltaTime);
      m_Renderer->BeginRenderPass();

      m_Renderer->Update(*m_Camera, deltaTime);
      m_ImGuiLayer->Update(m_Renderer->GetFrameIndex());

      m_Renderer->EndRenderPass();

      m_Window->SwapBuffers();
    }
  }
  m_Window->Shutdown();
  m_ImGuiLayer->Shutdown();
  m_Renderer->Shutdown();
}

DODO_END_NAMESPACE
