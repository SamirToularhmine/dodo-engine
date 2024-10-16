#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Debug/PerformanceManager.h>

#include <imgui.h>


DODO_BEGIN_NAMESPACE

namespace editor 
{
    static bool PROFILER_PANEL_OPENED = true;

    void ShowProfilerPanel() {
        // Performance stats window
        ImGui::Begin("Dodo Engine Performance stats", &PROFILER_PANEL_OPENED);

        ImGui::Text("Frame time: %f ms", PerformanceManager::GetFrameTime());
        for (const auto& [_name, _time] : PerformanceManager::GetTraces()) {
            ImGui::Text("%s: %f ms", _name.c_str(), _time);
        }

        ImGui::End();
    }
}

DODO_END_NAMESPACE