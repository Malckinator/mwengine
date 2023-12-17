#include "pch.h"
#include "vkFramebuffer.h"
#include "vkDisplay.h"

namespace mwengine::RenderAPI::Vulkan {
	vkFramebuffer::vkFramebuffer(Instance* instance, Display* display) {
		this->instance = (vkInstance*) instance;
		
		this->display = (vkDisplay*) display;
		this->imageIndex = 0;

		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(this->instance->GetNativeDevice(), &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
			MW_ERROR("Failed to create semaphore.");
		}
	}

	vkFramebuffer::~vkFramebuffer() {
		vkDestroySemaphore(instance->GetNativeDevice(), semaphore, nullptr);
	}

	Instance* vkFramebuffer::GetInstance() {
		return (Instance*) instance;
	}

	bool vkFramebuffer::IsFromDisplay() {
		return true;
	}

	glm::uvec2 vkFramebuffer::GetSize() {
		VkExtent2D extent = display->GetExtent2D();

		return { extent.width, extent.height };
	}

	void vkFramebuffer::NativeAdvance() {
		for (uint8 i = 0; i < 3; i++) { // try only three times to rebuild swapchain if can't get framebuffer
			VkResult result = vkAcquireNextImageKHR(instance->GetNativeDevice(), GetNativeSwapchain(), 1'000'000'000, semaphore, nullptr, &imageIndex);

			if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) {
				break;
			} else if (result == VK_ERROR_OUT_OF_DATE_KHR) {
				display->Rebuild();
				continue;
			} else {
				MW_ERROR("Failed to acquire swap chain image.");
			}
		}
	}

	VkSwapchainKHR vkFramebuffer::GetNativeSwapchain() {
		return display->GetNativeSwapchain();
	}

	VkRenderPass vkFramebuffer::GetNativeRenderPass() {
		return display->GetNativeRenderPass();
	}

	VkFramebuffer vkFramebuffer::GetNativeFramebuffer() {
		return display->GetNativeFrameBuffer(imageIndex);
	}

	uint32 vkFramebuffer::GetNativeImageIndex() {
		return imageIndex;
	}

	VkSemaphore vkFramebuffer::GetNativeSemaphore() {
		return semaphore;
	}
}