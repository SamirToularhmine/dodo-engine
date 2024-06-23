#pragma once

#include <DodoEngine/Core/Types.h>


DODO_BEGIN_NAMESPACE

class Renderer
{
public:
	virtual void Init(GLFWwindow* _nativeWindow) = 0;

	virtual void Update() = 0;

	virtual void Shutdown() = 0;

	virtual void DrawQuad() = 0;

	virtual ~Renderer() = default;
};

DODO_END_NAMESPACE