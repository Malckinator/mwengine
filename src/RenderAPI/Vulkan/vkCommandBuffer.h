#pragma once

#include "src/pch.h"
#include "vkDisplay.h"
#include "vkGraphicsPipeline.h"

namespace mwengine::RenderAPI::Vulkan {
	class vkCommandBuffer {
	public:
		vkCommandBuffer(vkInstance* instance);
		~vkCommandBuffer();

		void StartFrame(vkDisplay* display);
		void EndFrame();

		void QueueDraw(vkGraphicsPipeline* graphicsPipeline, vkBuffer* vertexBuffer = nullptr, vkBuffer* indexBuffer = nullptr, vkBuffer* instanceBuffer = nullptr);
	private:
		vkInstance* instance;
		VkCommandBuffer commandBuffer;
		VkSemaphore renderFinishedSemaphore;
		VkSemaphore imageAvailableSemaphore;
		VkFence inFlightFence;

		// Set in StartFrame()
		vkDisplay* display;
		uint32 imageIndex;
	};
}