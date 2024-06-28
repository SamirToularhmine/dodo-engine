#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Renderer/Renderer.h>
#include <DodoEngine/Renderer/UniformBufferObject.h>
#include <DodoEngine/Renderer/Vertex.h>

#include <GLFW/glfw3.h>


DODO_BEGIN_NAMESPACE


class VulkanRenderer : public Renderer
{
public:
	VulkanRenderer(VulkanContext& _vulkanContext);

	~VulkanRenderer() = default;

	void Init(GLFWwindow* _nativeWindow) override;

	void Update() override;

	void Shutdown() override;

	void DrawQuad() override;

private:
    static UniformBufferObject GetUniformBufferObject(const VulkanSwapChainData& _vulkanSwapChainData);

private:
	VulkanContext& m_VulkanContext;
	uint32_t m_FrameCount{ 0 };

	std::vector<VulkanBuffer> m_VertexBuffers;
	std::vector<VulkanBuffer> m_IndicesBuffers;
    std::vector<VulkanBuffer> m_UniformBuffers;

	Ref<VulkanDescriptorSet> m_VulkanDescriptorSet;

	UniformBufferObject m_UniformMvp;
};

DODO_END_NAMESPACE