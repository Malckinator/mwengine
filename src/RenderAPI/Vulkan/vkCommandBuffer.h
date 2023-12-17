#pragma once

#include "src/pch.h"
#include "vkDisplay.h"
#include "vkGraphicsPipeline.h"
#include "../CommandBuffer.h"
#include "vkFramebuffer.h"

namespace mwengine::RenderAPI::Vulkan {
	class vkCommandBuffer : CommandBuffer {
	public:
		vkCommandBuffer(Instance* instance);
		virtual ~vkCommandBuffer();

		virtual void StartFrame(Framebuffer* framebuffer);
		virtual void EndFrame();

		virtual void QueueDraw(
			GraphicsPipeline* graphicsPipeline,
			BufferRegion vertexBuffer = {},
			BufferRegion indexBuffer = {},
			BufferRegion instanceBuffer = {},
			BufferRegion uniformBuffer = {},
			std::vector<Texture*> textures = {}
		);
	private:
		vkInstance* instance;
		VkCommandBuffer commandBuffer;
		VkSemaphore renderFinishedSemaphore;
		VkFence inFlightFence;

		// Set in StartFrame()
		vkFramebuffer* framebuffer;
	};
}