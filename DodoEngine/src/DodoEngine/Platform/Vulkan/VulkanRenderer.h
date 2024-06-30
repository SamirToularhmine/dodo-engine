#pragma once

#include <DodoEngine/Core/Types.h>
#include <DodoEngine/Renderer/Renderer.h>
#include <DodoEngine/Renderer/UniformBufferObject.h>

#include <vector>

class GLFWwindow;

DODO_BEGIN_NAMESPACE

class Camera;
class VulkanBuffer;
class VulkanContext;
class VulkanDescriptorSet;
struct VulkanSwapChainData;

struct RendererData
{
    std::vector<glm::mat4> m_QuadTransforms;
};

class VulkanRenderer : public Renderer
{
public:
	VulkanRenderer(VulkanContext& _vulkanContext);

	~VulkanRenderer() override = default;

	void Init(GLFWwindow* _nativeWindow) override;

	void Update(const Camera& _camera, float _deltaTime) override;

	void Shutdown() override;

	void DrawQuad(const MeshTransform& _meshTransform) override;

private:
    static UniformBufferObject GetUniformBufferObject(const Camera& _camera, const VulkanSwapChainData& _vulkanSwapChainData, float _deltaTime);

private:
    VulkanContext& m_VulkanContext;
    uint32_t m_FrameCount{ 0 };

    std::vector<VulkanBuffer> m_VertexBuffers;
    std::vector<VulkanBuffer> m_IndicesBuffers;
    Ref<VulkanBuffer> m_UniformBuffer;

    Ref<VulkanDescriptorSet> m_VulkanDescriptorSet;

    UniformBufferObject m_UniformMvp;

    RendererData m_RendererData;

};

DODO_END_NAMESPACE