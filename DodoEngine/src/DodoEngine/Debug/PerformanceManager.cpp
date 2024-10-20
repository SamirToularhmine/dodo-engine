#include <DodoEngine/Debug/PerformanceManager.h>

DODO_BEGIN_NAMESPACE

struct PerformancesData
{
  PerformanceTraces m_PerformanceTraces;
  float m_FrameTime;
} s_Performances;

void PerformanceManager::Store(const std::string &_name, const float &_time)
{
  s_Performances.m_PerformanceTraces[_name] = _time;
}

void PerformanceManager::StoreFrameTime(float _frameTime)
{
  s_Performances.m_FrameTime = _frameTime;
}

PerformanceTraces PerformanceManager::GetTraces()
{
  return s_Performances.m_PerformanceTraces;
}

float PerformanceManager::GetFrameTime() { return s_Performances.m_FrameTime; }

void PerformanceManager::Clear() { s_Performances.m_PerformanceTraces.clear(); }

DODO_END_NAMESPACE