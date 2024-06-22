#pragma once

#include <DodoEngine/Utils/Utils.h>

class GLFWwindow;

DODO_BEGIN_NAMESPACE

class GraphicContext 
{
public:
    virtual void Init(GLFWwindow* _window) = 0;
    
    virtual void Update() = 0;

    virtual void Shutdown() = 0;

    virtual ~GraphicContext() = default;
};

DODO_END_NAMESPACE
