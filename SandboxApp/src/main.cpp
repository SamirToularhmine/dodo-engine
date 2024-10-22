#include <DodoEngine/DodoEngine.h>

#include <memory>

class DodoGameLayer : public dodo::GameLayer
{
public:
  DodoGameLayer(dodo::Ref<dodo::Application> _application) : dodo::GameLayer(_application) {}

  void Init() override
  {
    m_SphereModel = dodo::GltfLoader::LoadFromFile("/sphere/sphere.gltf");

    m_Scene = m_Application->GetCurrentScene();
    m_Scene->AddComponentToEntity<dodo::MeshComponent>(m_Scene->AddEntity("My first entity"),
                                                       {m_SphereModel});

    /*for (int i = 0; i < 10; ++i) {
      m_Renderer->DrawModel(sphere, {{i, 1, 1}, {0, 0, 0}, {0.5f, 0.5f,
    0.5f}}); m_Renderer->DrawModel(sphere, {{i, 1, -2}, {0, 0, 0}, {0.5f,
    0.5f, 0.5f}});
    }

    const float pos = easeInOutSine(rotateDegree / 500) * 10.0f;
    const glm::vec3 lightPos = {pos, 2, 5.0f};
    m_Renderer->DrawCube({lightPos, {0, 0, 0}, {0.25f, 0.25f, 0.25f}});

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
    }*/

    // float rotateDegree = 1;
    // Ref<Model> hasbullah =
    // std::make_shared<Model>(FbxLoader::LoadFromFile("resources/models/Hasbullah.fbx"));
    // Ref<Mesh> vikingRoom =
    // ObjLoader::LoadFromFile("resources/models/viking/VikingRoom.obj",
    // "resources/models/viking/viking_room.png"); Ref<Mesh> chestnut =
    // ObjLoader::LoadFromFile("resources/models/chestnut/chestnut.obj",
    // "resources/models/chestnut/model_Fire_emissive_transparent_color.tga.png");
    // Ref<Mesh> cube =
    // ObjLoader::LoadFromFile("resources/models/cube/Cube.obj",
    // "resources/models/cube/Cube.png"); Ref<Model> animatedCube =
    // std::make_shared<Model>(ModelIdProvider::GetId(),
    // GltfLoader::LoadFromFile("resources/models/animated-cube/AnimatedCube.gltf"));
    // Ref<Model> chessGame =
    // std::make_shared<Model>(ModelIdProvider::GetId(),
    // GltfLoader::LoadFromFile("resources/models/beautiful-game/ABeautifulGame.gltf"));
    // Ref<Model> tree = std::make_shared<Model>(ModelIdProvider::GetId(),
    // GltfLoader::LoadFromFile("resources/models/tree/tree.gltf"));
    // Ref<Model> avocado =
    // std::make_shared<Model>(GltfLoader::LoadFromFile("resources/models/avocado/Avocado.gltf"));
    // Ref<Model> sponza =
    // std::make_shared<Model>(GltfLoader::LoadFromFile("resources/models/sponza/Sponza.gltf"));
    // Ref<Model> sphere =
    // std::make_shared<Model>(GltfLoader::LoadFromFile("resources/models/sphere/sphere.gltf"));
    // const float movingSpeed = 0.1f;
  }

  void Update(float _deltaTime) override {}

  void Shutdown() override { m_SphereModel.reset(); }

private:
  dodo::Ref<dodo::Scene> m_Scene;
  dodo::Ref<dodo::Model> m_SphereModel;
};

int main(int argc, char const *argv[])
{
  srand((unsigned)time(NULL));

  auto application = dodo::Application::Create();

  dodo::Ref<DodoGameLayer> dodoGameLayer = std::make_shared<DodoGameLayer>(application);
  application->Init(dodo::WindowProps{1920, 1080, "Dodo Engine", true, false});
  application->AttachGameLayer(dodoGameLayer);

  application->Run();

  return 0;
}
