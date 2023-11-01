#pragma once

#include "src/pch.h"
#include "vulkan/vulkan.h"

namespace mwengine::RenderAPI::Vulkan {
	class vkFrameBuffer {
	public:
		vkFrameBuffer(VkDevice device, VkExtent2D extent, VkImageView imageView, VkRenderPass renderPass);
		~vkFrameBuffer();
	private:
		VkFramebuffer frameBuffer;
		VkDevice device;
	};
}