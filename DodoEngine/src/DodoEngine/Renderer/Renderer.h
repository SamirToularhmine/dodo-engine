#pragma once

#include <DodoEngine/Core/Types.h>

DODO_BEGIN_NAMESPACE

class Camera;
struct Light;
struct Model;
struct MeshTransform;
class Window;

class Renderer {
 public:
  static constexpr uint32_t MAX_MODEL_COUNT = 1'021;

 public:
  virtual void Init(const Window& _window) = 0;

  virtual void Update(const Camera& _camera, const Light& _light, float _deltaTime) = 0;

  virtual void Shutdown() = 0;

  virtual void RegisterModel(Ref<Model>& _mesh) = 0;

  virtual void DrawQuad(const MeshTransform& _meshTransform) = 0;

  virtual void DrawCube(const MeshTransform& _meshTransform) = 0;

  virtual void DrawModel(Ref<Model>& _model, const MeshTransform& _meshTransform) = 0;

  virtual void BeginRenderPass() = 0;

  virtual void EndRenderPass() = 0;

  virtual uint32_t GetFrameIndex() const = 0;

  virtual ~Renderer() = default;
};

DODO_END_NAMESPACE