#pragma once

#include <DodoEngine/Core/Types.h>

DODO_BEGIN_NAMESPACE

class Camera;
struct Light;
struct Frame;
struct Model;
struct MeshTransform;
struct RenderPass;
class Window;

class Renderer {
 public:
  static constexpr uint32_t MAX_MODEL_COUNT = 1'021;

      public : virtual void
               Init(const Window& _window) = 0;
  virtual void Update(Frame& _frame, const Camera& _camera, const Light& _light, float _deltaTime) = 0;
  virtual void Shutdown() = 0;

  virtual void RegisterModel(Ref<Model>& _mesh) = 0;

  virtual void DrawQuad(const MeshTransform& _meshTransform) = 0;
  virtual void DrawCube(const MeshTransform& _meshTransform) = 0;
  virtual void DrawModel(Ref<Model>& _model, const MeshTransform& _meshTransform) = 0;

  virtual RenderPass BeginUIRenderPass(Frame& _frame) = 0;
  virtual void EndUIRenderPass(const RenderPass& _renderPass) = 0;

  virtual Frame BeginFrame(const uint32_t& _frameNumber) = 0;
  virtual void EndFrame(const Frame& _frame) = 0;

  virtual ~Renderer() = default;
};

DODO_END_NAMESPACE