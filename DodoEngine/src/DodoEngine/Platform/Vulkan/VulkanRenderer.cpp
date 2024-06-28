#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>

#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Renderer/Quad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>


DODO_BEGIN_NAMESPACE

VulkanRenderer::VulkanRenderer(VulkanContext& _vulkanContext)
	: m_VulkanContext(_vulkanContext)
{
}

void VulkanRenderer::Init(GLFWwindow* _nativeWindow)
{
	m_VulkanContext.Init(_nativeWindow);

	// Quad vertex buffer
	m_VertexBuffers.emplace_back(
		QUAD_VERTICES.data(), sizeof(Vertex) * QUAD_VERTICES.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	m_IndicesBuffers.emplace_back(
		QUAD_INDICES.data(), sizeof(uint16_t) * QUAD_INDICES.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

    m_UniformBuffers.emplace_back(
		&m_UniformMvp, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
}

void VulkanRenderer::Update()
{
	VulkanRenderPassData vulkanRenderPassData{.m_FrameCount = m_FrameCount++};
	m_VulkanContext.BeginRenderPass(vulkanRenderPassData);

	if(!vulkanRenderPassData.m_RenderPassStarted)
	{
		return;
	}

	const Ref<VulkanDescriptorSet>& descriptorSet = vulkanRenderPassData.m_DescriptorSet;
	const VkCommandBuffer& commandBuffer = vulkanRenderPassData.m_CommandBuffer;

	m_UniformMvp = GetUniformBufferObject(vulkanRenderPassData.m_SwapChainData);
	m_UniformBuffers.front().SetMemory(&m_UniformMvp, sizeof(UniformBufferObject));
	descriptorSet->SetMemory(m_UniformBuffers, vulkanRenderPassData.m_FrameIndex);

	for(uint32_t  i = 0; i < m_VertexBuffers.size(); ++i)
	{
		const VulkanBuffer& _vertexBuffer = m_VertexBuffers[i];
		const VulkanBuffer& _indexBuffer = m_IndicesBuffers[i];

		VkBuffer vertexBuffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		descriptorSet->Bind(vulkanRenderPassData);

		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(QUAD_INDICES.size()), 1, 0, 0, 0);
	}

	m_VulkanContext.EndRenderPass(vulkanRenderPassData);
}

void VulkanRenderer::Shutdown()
{
	m_VulkanContext.Shutdown();
}

void VulkanRenderer::DrawQuad()
{
}

UniformBufferObject VulkanRenderer::GetUniformBufferObject(const VulkanSwapChainData& _vulkanSwapChainData) {
    const VkExtent2D extent = _vulkanSwapChainData.m_VkExtent;
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
	projectionMatrix[1][1] *= -1;

    return {
        glm::rotate(glm::mat4(1.0f), 1 * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		projectionMatrix    
    };
}

DODO_END_NAMESPACE
