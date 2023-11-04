#pragma once

#include "vkInstance.h"

namespace mwengine::RenderAPI::Vulkan {
	class vkBuffer {
	public:
		vkBuffer(vkInstance* instance, uint32 size);
		~vkBuffer();

		void* MapMemory(uint32 size = 0, uint32 offset = 0);
		void UnmapMemory(void* data);

		uint32 GetSize();
		VkBuffer GetNativeBuffer();
		VkDeviceMemory GetNativeDeviceMemory();
	private:
		uint32 FindMemoryType(uint32 typeFilter, VkMemoryPropertyFlags properties);

		vkInstance* instance;
		VkBuffer buffer;
		VkDeviceMemory deviceMemory;
		uint32 size;
	};
}