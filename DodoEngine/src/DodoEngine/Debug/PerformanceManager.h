#pragma once

#include <DodoEngine/Core/Types.h>

#include <unordered_map>
#include <string>

DODO_BEGIN_NAMESPACE

using PerformanceTraces = std::unordered_map<std::string, float>;

class PerformanceManager {
 public:
  static void Store(const std::string& _name, const float& _time);
  static void StoreFrameTime(float _frameTime);
  static PerformanceTraces GetTraces();
  static float GetFrameTime();
  static void Clear();
};

DODO_END_NAMESPACE