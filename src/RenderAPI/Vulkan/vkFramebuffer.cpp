#include "pch.h"
#include "vkFramebuffer.h"
#include "vkDisplay.h"
#include "vkUtils.h"

namespace mwengine::RenderAPI::Vulkan {
    vkFramebuffer::vkFramebuffer(Instance* instance, Image* image, Image* depthImage) {
		size = image->GetResolution();
		this->instance = (vkInstance*) instance;
		this->fromDisplay = false;

		VkAttachmentDescription colorAttachment {};
		colorAttachment.format = VK_FORMAT_R8G8B8A8_SRGB;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachment {};
		depthAttachment.format = Utils::GetDepthFormat((vkInstance*) instance);
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 2;
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(((vkInstance*) instance)->GetNativeDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			MW_ERROR("Failed to create render pass.");
		}

		VkImageView attachments2[] = {
			((vkImage*) image)->GetNativeImageView(),
			((vkImage*) depthImage)->GetNativeImageView()
		};

		VkFramebufferCreateInfo framebufferInfo {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = (depthImage == nullptr) ? 1 : 2;
		framebufferInfo.pAttachments = attachments2;
		framebufferInfo.width = size.x;
		framebufferInfo.height = size.y;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(this->instance->GetNativeDevice(), &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
			MW_ERROR("Failed to create framebuffer.");
		}
    }

    vkFramebuffer::vkFramebuffer(Instance* instance, Display* display) {
		VkExtent2D extent = ((vkDisplay*) display)->GetExtent2D();

		this->instance = (vkInstance*) instance;
		this->fromDisplay = true;
		this->size = { extent.width, extent.height };
		
		this->display = (vkDisplay*) display;
		this->imageIndex = 0;

		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(this->instance->GetNativeDevice(), &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
			MW_ERROR("Failed to create semaphore.");
		}
	}

	vkFramebuffer::~vkFramebuffer() {
		if (fromDisplay) {
			vkDestroySemaphore(instance->GetNativeDevice(), semaphore, nullptr);
		} else {
			vkDestroyFramebuffer(instance->GetNativeDevice(), framebuffer, nullptr);
			vkDestroyRenderPass(instance->GetNativeDevice(), renderPass, nullptr);
		}
	}

	Instance* vkFramebuffer::GetInstance() {
		return (Instance*) instance;
	}

	bool vkFramebuffer::IsFromDisplay() {
		return fromDisplay;
	}

	glm::uvec2 vkFramebuffer::GetSize() {
		return size;
	}

	void vkFramebuffer::NativeAdvance() {
		if (!fromDisplay)
			return;

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
		if (fromDisplay)
			return display->GetNativeRenderPass();
		return renderPass;
	}

	VkFramebuffer vkFramebuffer::GetNativeFramebuffer() {
		if (fromDisplay)
			return display->GetNativeFrameBuffer(imageIndex);
		return framebuffer;
	}

	uint32 vkFramebuffer::GetNativeImageIndex() {
		return imageIndex;
	}

	VkSemaphore vkFramebuffer::GetNativeSemaphore() {
		return semaphore;
	}
}