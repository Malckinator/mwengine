#include "pch.h"
#include "vkBuffer.h"

namespace mwengine::RenderAPI::Vulkan {
	vkBuffer::vkBuffer(vkInstance* instance, uint32 size) {
		this->instance = instance;
		this->size = size;

		VkBufferCreateInfo bufferInfo {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(instance->GetNativeDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			MW_ERROR("Failed to create buffer.");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(instance->GetNativeDevice(), buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(instance->GetNativeDevice(), &allocInfo, nullptr, &deviceMemory) != VK_SUCCESS) {
			MW_ERROR("Failed to allocate device memory.");
		}

		vkBindBufferMemory(instance->GetNativeDevice(), buffer, deviceMemory, 0);
	}

	vkBuffer::~vkBuffer() {
		vkDestroyBuffer(instance->GetNativeDevice(), buffer, nullptr);
		vkFreeMemory(instance->GetNativeDevice(), deviceMemory, nullptr);
	}

	void* vkBuffer::MapMemory(uint32 size, uint32 offset) {
		if (size == 0)
			size = this->size;

		void* data;
		vkMapMemory(instance->GetNativeDevice(), deviceMemory, offset, size, 0, &data);
		return data;
	}

	void vkBuffer::UnmapMemory(void* data) {
		vkUnmapMemory(instance->GetNativeDevice(), deviceMemory);
	}

	uint32 vkBuffer::GetSize() {
		return size;
	}

	VkBuffer vkBuffer::GetNativeBuffer() {
		return buffer;
	}

	VkDeviceMemory vkBuffer::GetNativeDeviceMemory() {
		return deviceMemory;
	}

	uint32 vkBuffer::FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(instance->GetPhysicalDevice()->GetNativePhysicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		MW_ERROR("Failed to find suitible memory type.");
		return 0;
	}
}