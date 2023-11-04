#pragma once

#include "src/pch.h"
#include "vkInstance.h"

namespace mwengine::RenderAPI::Vulkan {
	class vkImageFormat {
	public:
		vkImageFormat(vkInstance* instance, VkRenderPass renderPass, VkExtent2D extent);
		~vkImageFormat();

		MWATH::Int2 GetSize();
		VkRenderPass GetNativeRenderPass();
	private:
		vkInstance* instance;
		VkRenderPass renderPass;
		MWATH::Int2 size;
	};
}