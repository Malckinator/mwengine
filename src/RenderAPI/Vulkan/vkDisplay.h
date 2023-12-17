#pragma once

#include "vkInstance.h"
#include "../Display.h"
#include "vkFramebuffer.h"

namespace mwengine::RenderAPI::Vulkan {
	class vkDisplay : Display {
	public:
		vkDisplay(Instance* instance, Window* window = nullptr);
		virtual ~vkDisplay();

		void Rebuild();

		virtual Instance* GetInstance();
		virtual Framebuffer* GetFramebuffer();

		VkExtent2D GetExtent2D();
		VkSwapchainKHR GetNativeSwapchain();
		VkFramebuffer GetNativeFrameBuffer(uint32 index);
		VkRenderPass GetNativeRenderPass();
	private:
		void CreateSwapChain();
		void DestroySwapChain();
		VkSurfaceFormatKHR GetSwapSurfaceFormat();
		VkPresentModeKHR GetSwapSurfacePresentMode();
		VkExtent2D GetSwapExtent();

		vkInstance* instance;
		Window* window;
		VkSurfaceKHR surface;
		bool inheritFromInstance;
		VkRenderPass renderPass;
		VkFormat format;
		VkExtent2D extent;
		VkSwapchainKHR swapchain;
		VkSemaphore imageAvailableSemaphore;
		Framebuffer* framebuffer;

		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
		std::vector<VkFramebuffer> framebuffers;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;
	};
}