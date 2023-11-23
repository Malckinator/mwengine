#include "pch.h"
#include "vkDisplay.h"
#include "vkPlatform.h"
#include "vkUtils.h"

namespace mwengine::RenderAPI::Vulkan {
	const std::vector<const char*> requiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	
	vkDisplay::vkDisplay(Instance* instance, Window* window) {
		if (window == nullptr) {
			inheritFromInstance = true;
			surface = ((vkInstance*) instance)->GetNativeSurface();
			this->window = window;
		} else {
			inheritFromInstance = false;
			this->window = window;
			surface = CreateSurface(window, ((vkInstance*) instance)->GetNativeInstance());
		}

		this->instance = (vkInstance*) instance;
		this->format = GetSwapSurfaceFormat().format;
		this->swapchain = nullptr;

		VkAttachmentDescription colorAttachment {};
		colorAttachment.format = format;
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

		VkRenderPass renderPass;

		if (vkCreateRenderPass(((vkInstance*) instance)->GetNativeDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			MW_ERROR("Failed to create render pass.");
		}

		imageFormat = new vkImageFormat((vkInstance*) instance, renderPass, extent);

		CreateSwapChain();
	}

	vkDisplay::~vkDisplay() {
		DestroySwapChain();

		vkDestroySwapchainKHR(instance->GetNativeDevice(), swapchain, nullptr);
		delete imageFormat;
		if (!inheritFromInstance)
			vkDestroySurfaceKHR(instance->GetNativeInstance(), surface, nullptr);
	}

	void vkDisplay::Rebuild() {
		vkDeviceWaitIdle(instance->GetNativeDevice());

		DestroySwapChain();
		CreateSwapChain();
	}

	Instance* vkDisplay::GetInstance() {
		return (Instance*) instance;
	}

	ImageFormat* vkDisplay::GetImageFormat() {
		return (ImageFormat*) imageFormat;
	}

	VkExtent2D vkDisplay::GetExtent2D() {
		return extent;
	}

	VkSwapchainKHR vkDisplay::GetNativeSwapchain() {
		return swapchain;
	}

	VkFramebuffer vkDisplay::GetNativeFrameBuffer(uint32 index) {
		return framebuffers[index];
	}

	void vkDisplay::CreateSwapChain() {
		VkSwapchainKHR oldSwapChain = swapchain;

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(((vkPhysicalDevice*) instance->GetPhysicalDevice())->GetNativePhysicalDevice(), surface, &capabilities);

		uint32 imageCount = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
			uint32 imageCount = capabilities.maxImageCount;
		}

		VkSurfaceFormatKHR imageFormat = GetSwapSurfaceFormat();
		format = imageFormat.format;
		extent = GetSwapExtent();

		VkSwapchainCreateInfoKHR createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = imageFormat.format;
		createInfo.imageColorSpace = imageFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32 queueFamilyIndices[] = { ((vkPhysicalDevice*) instance->GetPhysicalDevice())->GetGraphicsQueueFamily().value(), ((vkPhysicalDevice*) instance->GetPhysicalDevice())->GetPresentQueueFamily().value() };

		if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = GetSwapSurfacePresentMode();
		createInfo.clipped = true;
		createInfo.oldSwapchain = oldSwapChain;

		if (vkCreateSwapchainKHR(instance->GetNativeDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
			MW_ERROR("Failed to create swapchain.");
		}

		VkFormat depthFormat = Utils::GetDepthFormat(instance);
		Utils::CreateImage(
			instance,
			glm::uvec2(extent.width, extent.height),
			depthFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			depthImage,
			depthImageMemory
		);

		depthImageView = Utils::CreateImageView(instance, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		uint32 swapImageCount;
		vkGetSwapchainImagesKHR(instance->GetNativeDevice(), swapchain, &swapImageCount, nullptr);
		images.resize(swapImageCount);
		vkGetSwapchainImagesKHR(instance->GetNativeDevice(), swapchain, &swapImageCount, images.data());

		imageViews.resize(images.size());
		for (uint32 i = 0; i < images.size(); i++) {
			imageViews[i] = Utils::CreateImageView(instance, images[i], format, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		framebuffers.resize(imageViews.size());
		for (uint32 i = 0; i < imageViews.size(); i++) {
			VkImageView attachments[] = {
				imageViews[i],
				depthImageView
			};

			VkFramebufferCreateInfo framebufferInfo {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = this->imageFormat->GetNativeRenderPass();
			framebufferInfo.attachmentCount = 2;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = extent.width;
			framebufferInfo.height = extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(instance->GetNativeDevice(), &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
				MW_ERROR("Failed to create framebuffer.");
			}
		}

		if (oldSwapChain != nullptr)
			vkDestroySwapchainKHR(instance->GetNativeDevice(), oldSwapChain, nullptr);
	}

	void vkDisplay::DestroySwapChain() {
		vkDestroyImageView(instance->GetNativeDevice(), depthImageView, nullptr);
		vkDestroyImage(instance->GetNativeDevice(), depthImage, nullptr);
		vkFreeMemory(instance->GetNativeDevice(), depthImageMemory, nullptr);

		for (VkFramebuffer framebuffer : framebuffers) {
			vkDestroyFramebuffer(instance->GetNativeDevice(), framebuffer, nullptr);
		}

		for (VkImageView imageView : imageViews) {
			vkDestroyImageView(instance->GetNativeDevice(), imageView, nullptr);
		}
	}

	VkSurfaceFormatKHR vkDisplay::GetSwapSurfaceFormat() {
		uint32 formatCount;
		std::vector<VkSurfaceFormatKHR> formats;
		vkGetPhysicalDeviceSurfaceFormatsKHR(((vkPhysicalDevice*) instance->GetPhysicalDevice())->GetNativePhysicalDevice(), surface, &formatCount, nullptr);

		if (formatCount != 0) {
			formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(((vkPhysicalDevice*) instance->GetPhysicalDevice())->GetNativePhysicalDevice(), surface, &formatCount, formats.data());

			for (const auto& format : formats) {
				if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
					return format;
				}
			}

			return formats[0];
		}

		return {};
	}

	VkPresentModeKHR vkDisplay::GetSwapSurfacePresentMode() {
		uint32 modeCount;
		std::vector<VkPresentModeKHR> modes;
		vkGetPhysicalDeviceSurfacePresentModesKHR(((vkPhysicalDevice*) instance->GetPhysicalDevice())->GetNativePhysicalDevice(), surface, &modeCount, nullptr);

		if (modeCount != 0) {
			modes.resize(modeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(((vkPhysicalDevice*) instance->GetPhysicalDevice())->GetNativePhysicalDevice(), surface, &modeCount, modes.data());

			for (const auto& mode : modes) {
				if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
					return mode;
				}
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D vkDisplay::GetSwapExtent() {
		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(((vkPhysicalDevice*) instance->GetPhysicalDevice())->GetNativePhysicalDevice(), surface, &capabilities);

		if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max()) {
			return capabilities.currentExtent;
		} else {
			glm::uvec2 size = window->GetClientSize();

			return {
				std::clamp((uint32) size.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
				std::clamp((uint32) size.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
			};
		}
	}
}