#pragma once

#include <DodoEngine/Core/Types.h>


class GLFWwindow;

DODO_BEGIN_NAMESPACE

class Camera;
struct MeshTransform;

class Renderer
{
public:
	virtual void Init(GLFWwindow* _nativeWindow) = 0;

	virtual void Update(const Camera& _camera, float _deltaTime) = 0;

	virtual void Shutdown() = 0;

	virtual void DrawQuad(const MeshTransform& _meshTransform) = 0;

	virtual ~Renderer() = default;
};

DODO_END_NAMESPACE