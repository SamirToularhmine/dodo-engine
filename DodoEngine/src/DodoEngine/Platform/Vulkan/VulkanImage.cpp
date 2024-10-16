#include <DodoEngine/Platform/Vulkan/VulkanImage.h>

#include <DodoEngine/Platform/Vulkan/VulkanBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanCommandBuffer.h>
#include <DodoEngine/Platform/Vulkan/VulkanContext.h>
#include <DodoEngine/Platform/Vulkan/VulkanDevice.h>
#include <DodoEngine/Platform/Vulkan/VulkanPhysicalDevice.h>
#include <DodoEngine/Utils/Log.h>

#include <vk_mem_alloc.h>

DODO_BEGIN_NAMESPACE

VulkanImage::VulkanImage(uint32_t _imageWidth, uint32_t _imageHeight, VkImageTiling _tiling, uint32_t _usage, uint32_t _featureFlags)
    : m_Width(_imageWidth), m_Height(_imageHeight), m_Tiling(_tiling), m_Usage(_usage) {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  const Ref<VulkanPhysicalDevice> vulkanPhysicalDevice = vulkanContext.GetVulkanPhysicalDevice();
  const std::vector<VkFormat> candidates = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};

  for (VkFormat format : candidates) {
    VkFormatProperties properties = vulkanPhysicalDevice->GetFormatProperties(format);

    if (_tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & _featureFlags) == _featureFlags) {
      m_Format = format;
      break;
    } else if (_tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & _featureFlags) == _featureFlags) {
      m_Format = format;
      break;
    }
  }
  
  CreateImage(_imageWidth, _imageHeight, _tiling, _usage, m_Format);
}

VulkanImage::VulkanImage(uint32_t _imageWidth, uint32_t _imageHeight, VkImageTiling _tiling, uint32_t _usage, VkFormat _format)
    : m_Width(_imageWidth), m_Height(_imageHeight), m_Tiling(_tiling), m_Usage(_usage), m_Format(_format) {
  CreateImage(_imageWidth, _imageHeight, _tiling, _usage, _format);
}

void VulkanImage::CreateImage(uint32_t _imageWidth, uint32_t _imageHeight, VkImageTiling _tiling, uint32_t _usage, VkFormat _format) {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  const VmaAllocator& allocator = vulkanContext.GetAllocator();

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = static_cast<uint32_t>(_imageWidth);
  imageInfo.extent.height = static_cast<uint32_t>(_imageHeight);
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = _format;
  imageInfo.tiling = _tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = _usage;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.flags = 0;

  VmaAllocationCreateInfo vmaAllocInfo{};
  vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

  VkResult createImageResult = vmaCreateImage(allocator, &imageInfo, &vmaAllocInfo, &m_Image, &m_Allocation, nullptr);
  if (createImageResult != VK_SUCCESS) {
    DODO_ERROR("Couldn't create texture image");
  }

  m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
}

VulkanImage::~VulkanImage() {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  const VmaAllocator& allocator = vulkanContext.GetAllocator();

  vmaDestroyImage(allocator, m_Image, m_Allocation);
}

void VulkanImage::TransitionImageLayout(VkImageLayout _newLayout) {
  const VulkanContext& vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice>& vulkanDevice = vulkanContext.GetVulkanDevice();
  VulkanCommandBuffer commandBuffer;
  const VulkanDevice::Queues& queues = vulkanDevice->GetQueues();

  commandBuffer.BeginRecording();

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = m_Layout;
  barrier.newLayout = _newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = m_Image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags sourceStage{};
  VkPipelineStageFlags destinationStage{};

  if (m_Layout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (m_Layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    DODO_ERROR("Invalid layout transition");
  }

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

  commandBuffer.EndRecording();
  commandBuffer.Submit(queues.m_GraphicsFamilyQueue);

  m_Layout = _newLayout;
}

void VulkanImage::CopyFromBuffer(const VulkanBuffer& _imageBuffer) {

  const VulkanContext& vulkanContext = VulkanContext::Get();
  const Ref<VulkanDevice>& vulkanDevice = vulkanContext.GetVulkanDevice();
  const VkCommandPool& vkCommandPool = vulkanContext.GetCommandPool();

  VulkanCommandBuffer commandBuffer;
  commandBuffer.BeginRecording();

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height), 1};

  vkCmdCopyBufferToImage(commandBuffer, _imageBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  commandBuffer.EndRecording();
  commandBuffer.Submit(vulkanDevice->GetQueues().m_GraphicsFamilyQueue);
}

DODO_END_NAMESPACE
