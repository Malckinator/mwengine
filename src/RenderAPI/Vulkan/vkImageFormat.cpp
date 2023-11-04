#include "pch.h"
#include "vkImageFormat.h"

namespace mwengine::RenderAPI::Vulkan {
	vkImageFormat::vkImageFormat(vkInstance* instance, VkRenderPass renderPass, VkExtent2D extent) {
		this->instance = instance;
		this->renderPass = renderPass;
		this->size = MWATH::Int2(extent.width, extent.height);
	}

	vkImageFormat::~vkImageFormat() {
		vkDestroyRenderPass(instance->GetNativeDevice(), renderPass, nullptr);
	}

    MWATH::Int2 vkImageFormat::GetSize() {
        return size;
    }

    VkRenderPass vkImageFormat::GetNativeRenderPass() {
		return renderPass;
	}
}