#pragma once

#include <DodoEngine/Utils/Utils.h>

DODO_BEGIN_NAMESPACE

class Window;

class GraphicContext
{
public:
  virtual void Init(const Window &_window) = 0;

  virtual void Update(uint32_t _frameId) = 0;

  virtual void Shutdown() = 0;

  virtual ~GraphicContext() = default;
};

DODO_END_NAMESPACE
