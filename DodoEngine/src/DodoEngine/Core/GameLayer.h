#pragma once

#include <DodoEngine/Core/Types.h>

DODO_BEGIN_NAMESPACE

class Application;

class GameLayer
{
public:
  GameLayer(Ref<Application> &_application) : m_Application(_application) {}

  virtual void Init() = 0;
  virtual void Update(float _deltaTime) = 0;
  virtual void Shutdown() = 0;

protected:
  Ref<Application> m_Application;
};

DODO_END_NAMESPACE