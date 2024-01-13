#pragma once

#include "src/pch.h"
#include "vulkan/vulkan.h"
#include "../Framebuffer.h"
#include "vkInstance.h"
#include "vkImage.h"

namespace mwengine::RenderAPI {
	class Display;
}

namespace mwengine::RenderAPI::Vulkan {
	class vkDisplay;

	class vkFramebuffer : Framebuffer {
	public:
		vkFramebuffer(Instance* instance, Image* image, Image* depthImage = nullptr);
		vkFramebuffer(Instance* instance, Display* display);
		virtual ~vkFramebuffer();

		virtual Instance* GetInstance();
		virtual bool IsFromDisplay();
		virtual Math::UInt2 GetSize();

		void NativeAdvance();
		VkSwapchainKHR GetNativeSwapchain();
		VkRenderPass GetNativeRenderPass();
		VkFramebuffer GetNativeFramebuffer();
		uint32 GetNativeImageIndex();
		VkSemaphore GetNativeSemaphore();
	private:
		vkInstance* instance;
		bool fromDisplay;

		union {
			struct {
				vkDisplay* display;
				uint32 imageIndex;
				VkSemaphore semaphore;
			};

			struct {
				VkFramebuffer framebuffer;
				Math::UInt2 size;
				VkRenderPass renderPass;
			};
		};
	};
}