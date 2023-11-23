#pragma once

#include "src/pch.h"
#include "vkInstance.h"
#include "../ImageFormat.h"

namespace mwengine::RenderAPI::Vulkan {
	class vkImageFormat : ImageFormat {
	public:
		vkImageFormat(vkInstance* instance, VkRenderPass renderPass, VkExtent2D extent);
		virtual ~vkImageFormat();

		virtual glm::uvec2 GetSize();

		VkRenderPass GetNativeRenderPass();
	private:
		vkInstance* instance;
		VkRenderPass renderPass;
		glm::uvec2 size;
	};
}