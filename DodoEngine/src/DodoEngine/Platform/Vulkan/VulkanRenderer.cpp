#include <DodoEngine/Platform/Vulkan/VulkanRenderer.h>

#include <DodoEngine/Core/Camera.h>
#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDescriptorSet.h>
#include <DodoEngine/Renderer/Quad.h>
#include <DodoEngine/Renderer/UniformBufferObject.h>

#include <GLFW/glfw3.h>
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

	const Ref<VulkanDevice>& vulkanDevice = m_VulkanContext.GetVulkanDevice();
	const VulkanPhysicalDevice& vulkanPhysicalDevice = *m_VulkanContext.GetVulkanPhysicalDevice();

	constexpr VkMemoryPropertyFlags memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	// Quad Vertex buffer
	m_VertexBuffers.emplace_back(
		 VulkanBufferSpec { sizeof(Vertex) * QUAD_VERTICES.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryPropertyFlags }, vulkanDevice, vulkanPhysicalDevice
	);
	m_VertexBuffers[0].SetMemory(QUAD_VERTICES.data(), sizeof(Vertex) * QUAD_VERTICES.size());

	// Quad index buffer
	m_IndicesBuffers.emplace_back(
		VulkanBufferSpec { sizeof(uint16_t) * QUAD_INDICES.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memoryPropertyFlags }, vulkanDevice, vulkanPhysicalDevice
	);
	m_IndicesBuffers[0].SetMemory(QUAD_INDICES.data(), sizeof(uint16_t) * QUAD_INDICES.size());

	// Uniform buffer
	m_UniformBuffer = std::make_shared<VulkanBuffer>(VulkanBufferSpec { sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, memoryPropertyFlags }, vulkanDevice, vulkanPhysicalDevice);
}

void VulkanRenderer::Update(const Camera& _camera, float _deltaTime)
{
	VulkanRenderPassData vulkanRenderPassData{m_FrameCount++};
	m_VulkanContext.BeginRenderPass(vulkanRenderPassData);

	if(!vulkanRenderPassData.m_RenderPassStarted)
	{
		return;
	}
	
	const Ref<VulkanDescriptorSet>& descriptorSet = vulkanRenderPassData.m_DescriptorSet;
	const VkCommandBuffer& commandBuffer = vulkanRenderPassData.m_CommandBuffer;

	m_UniformMvp = GetUniformBufferObject(_camera, vulkanRenderPassData.m_SwapChainData, _deltaTime);
	m_UniformBuffer->SetMemory(&m_UniformMvp, sizeof(UniformBufferObject));
	descriptorSet->UpdateDescriptor(*m_UniformBuffer, vulkanRenderPassData.m_FrameIndex);

	for(uint32_t  i = 0; i < m_VertexBuffers.size(); ++i)
	{
		const VulkanBuffer& _vertexBuffer = m_VertexBuffers[i];
		const VulkanBuffer& _indexBuffer = m_IndicesBuffers[i];

		VkBuffer vertexBuffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		descriptorSet->Bind(vulkanRenderPassData);

		vkCmdDrawIndexed(commandBuffer, QUAD_INDICES.size(), 1, 0, 0, 0);
	}

	m_VulkanContext.EndRenderPass(vulkanRenderPassData);
}

void VulkanRenderer::Shutdown()
{
	m_VulkanContext.Shutdown();
}

void VulkanRenderer::DrawQuad(const MeshTransform& _meshTransform)
{
	
}

UniformBufferObject VulkanRenderer::GetUniformBufferObject(const Camera& _camera, const VulkanSwapChainData& _vulkanSwapChainData, float _deltaTime) {
    const VkExtent2D extent = _vulkanSwapChainData.m_VkExtent;
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), extent.width / static_cast<float>(extent.height), 0.1f, 10.0f);
	projectionMatrix[1][1] *= -1;

    return {
        rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		_camera.GetViewMatrix(),
		projectionMatrix    
    };
}

DODO_END_NAMESPACE
