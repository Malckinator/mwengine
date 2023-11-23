#pragma once

#include "vkInstance.h"
#include "vkImageFormat.h"
#include "../Display.h"

namespace mwengine::RenderAPI::Vulkan {
	class vkDisplay : Display {
	public:
		vkDisplay(Instance* instance, Window* window = nullptr);
		virtual ~vkDisplay();

		virtual void Rebuild();

		virtual Instance* GetInstance();
		virtual ImageFormat* GetImageFormat();

		VkExtent2D GetExtent2D();
		VkSwapchainKHR GetNativeSwapchain();
		VkFramebuffer GetNativeFrameBuffer(uint32 index);
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
		vkImageFormat* imageFormat;
		VkFormat format;
		VkExtent2D extent;
		VkSwapchainKHR swapchain;

		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
		std::vector<VkFramebuffer> framebuffers;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;
	};
}