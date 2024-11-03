#ifndef PROFILE_CPP
#define PROFILE_CPP

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Debug/PerformanceManager.h>

#include <imgui.h>

#include <array>

DODO_BEGIN_NAMESPACE

namespace editor
{
static bool PROFILER_OPENED = true;
static std::array<float, 100> s_FrameTimes{0};
static uint64_t s_FrameNumber = 0;

void ShowProfiler()
{
  // Performance stats window
  uint32_t frameIndex = s_FrameNumber++ % 100;
  s_FrameTimes[frameIndex] = PerformanceManager::GetFrameTime() * 10000;

  ImGui::Begin("Dodo Engine Performance stats", &PROFILER_OPENED);
  {
    for (const auto &[_name, _time] : PerformanceManager::GetTraces())
    {
      ImGui::Text("%s: %f ms", _name.c_str(), _time);
    }

    ImGui::PlotLines("Frametime", s_FrameTimes.data(), 100);
  }

  ImGui::End();
}
} // namespace editor

DODO_END_NAMESPACE

#endif
