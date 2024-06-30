#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>

#include <cstdint>
#include <string>

class GLFWwindow;

DODO_BEGIN_NAMESPACE

struct WindowProps {
    uint32_t m_Width{800};
    uint32_t m_Height{600};
    std::string m_Name{"Dodo Engine"};
    bool m_VSync{ false };
};

class Window
{
public:
    void Init(const WindowProps& _windowProps);

	bool ShouldClose() const;

	void SwapBuffers() const;

	void PollEvents() const;

    void Update() const;

	void Shutdown() const;

    float GetTime() const;

	GLFWwindow* GetNativeWindow() const;
    
    private:
        WindowProps m_Props{};
        GLFWwindow* m_NativeWindow{nullptr};
        VulkanContext m_VulkanContext;
};

DODO_END_NAMESPACE