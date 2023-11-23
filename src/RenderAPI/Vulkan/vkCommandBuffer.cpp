#include "pch.h"
#include "vkCommandBuffer.h"

namespace mwengine::RenderAPI::Vulkan {
	vkCommandBuffer::vkCommandBuffer(vkInstance* instance) {
		this->instance = instance;
		this->renderFinishedSemaphore = nullptr;
		this->inFlightFence = nullptr;

		VkCommandBufferAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = instance->GetNativeCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(instance->GetNativeDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
			MW_ERROR("Failed to allocate command buffer.");
		}

		// fence and semaphore

		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateSemaphore(instance->GetNativeDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(instance->GetNativeDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateFence(instance->GetNativeDevice(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
			MW_ERROR("Failed to create semaphores.");
		}
	}

	vkCommandBuffer::~vkCommandBuffer() {
		vkDeviceWaitIdle(instance->GetNativeDevice());

		vkDestroySemaphore(instance->GetNativeDevice(), imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(instance->GetNativeDevice(), renderFinishedSemaphore, nullptr);
		vkDestroyFence(instance->GetNativeDevice(), inFlightFence, nullptr);
		vkFreeCommandBuffers(instance->GetNativeDevice(), instance->GetNativeCommandPool(), 1, &commandBuffer);
	}

	void vkCommandBuffer::StartFrame(vkDisplay* display) {
		for (uint8 i = 0; i < 3; i++) { // try only three times to rebuild swapchain if can't get framebuffer
			VkResult result = vkAcquireNextImageKHR(instance->GetNativeDevice(), display->GetNativeSwapchain(), 1'000'000'000, imageAvailableSemaphore, nullptr, &imageIndex);

			if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) {
				break;
			} else if (result == VK_ERROR_OUT_OF_DATE_KHR) {
				display->Rebuild();
				continue;
			} else {
				MW_ERROR("Failed to acquire swap chain image.");
			}
		}

		vkWaitForFences(instance->GetNativeDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(instance->GetNativeDevice(), 1, &inFlightFence);
		vkResetCommandBuffer(commandBuffer, 0);

		this->display = display;

		VkCommandBufferBeginInfo beginInfo {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			MW_ERROR("Failed to begin recording command buffer.");
		}

		VkRenderPassBeginInfo renderPassInfo {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = display->GetImageFormat()->GetNativeRenderPass();
		renderPassInfo.framebuffer = display->GetNativeFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = display->GetExtent2D();

		VkClearValue clearValues[2] {};
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void vkCommandBuffer::EndFrame() {
		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			MW_ERROR("Failed to record command buffer.");
		}

		VkSubmitInfo submitInfo {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VkQueue graphicsQueue;
		vkGetDeviceQueue(instance->GetNativeDevice(), ((vkPhysicalDevice*) instance->GetPhysicalDevice())->GetGraphicsQueueFamily().value(), 0, &graphicsQueue);

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
			MW_ERROR("Failed to submit draw command buffer.");
		}

		VkSwapchainKHR swapchains[] = {display->GetNativeSwapchain()};

		VkPresentInfoKHR presentInfo {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		VkQueue presentQueue;
		vkGetDeviceQueue(display->GetInstance()->GetNativeDevice(), ((vkPhysicalDevice*) instance->GetPhysicalDevice())->GetPresentQueueFamily().value(), 0, &presentQueue);

		vkQueuePresentKHR(presentQueue, &presentInfo);

		this->display = nullptr;
		this->imageIndex = 0;
	}

	void vkCommandBuffer::QueueDraw(
		vkGraphicsPipeline* graphicsPipeline,
		BufferRegion vertexBuffer,
		BufferRegion indexBuffer,
		BufferRegion instanceBuffer,
		BufferRegion uniformBuffer,
		std::vector<vkTexture*> textures
	) {
		VkExtent2D extent = display->GetExtent2D();

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetNativePipeline());

		if (graphicsPipeline->hasUniformBuffer || graphicsPipeline->textureCount != 0) {
			uint8 uniformBufferCount = graphicsPipeline->hasUniformBuffer ? 1 : 0;
			VkDescriptorSet descriptorSet = graphicsPipeline->GetNativeDescriptorSet();
			std::vector<VkDescriptorImageInfo> imageInfos(graphicsPipeline->textureCount);
			std::vector<VkWriteDescriptorSet> descriptorWrites((graphicsPipeline->textureCount != 0 ? 1 : 0) + uniformBufferCount);
			uint32 binding = 0;

			if (graphicsPipeline->hasUniformBuffer) {
				VkDescriptorBufferInfo bufferInfo {};
				bufferInfo.buffer = uniformBuffer.buffer->GetNativeBuffer();
				bufferInfo.offset = uniformBuffer.offset;
				bufferInfo.range = uniformBuffer.size;

				VkWriteDescriptorSet descriptorWrite {};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = descriptorSet;
				descriptorWrite.dstBinding = binding;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &bufferInfo;
				descriptorWrite.pImageInfo = nullptr;
				descriptorWrite.pTexelBufferView = nullptr;

				descriptorWrites[binding] = descriptorWrite;
				binding++;
			}

			if (graphicsPipeline->textureCount != 0) {
				for (uint32 i = 0; i < textures.size(); i++) {
					VkDescriptorImageInfo imageInfo {};
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView = textures[i]->GetImage()->GetNativeImageView();
					imageInfo.sampler = textures[i]->GetNativeSampler();

					imageInfos[i] = imageInfo;
				}

				VkWriteDescriptorSet descriptorWrite {};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = descriptorSet;
				descriptorWrite.dstBinding = binding;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrite.descriptorCount = graphicsPipeline->textureCount;
				descriptorWrite.pBufferInfo = nullptr;
				descriptorWrite.pImageInfo = imageInfos.data();
				descriptorWrite.pTexelBufferView = nullptr;

				descriptorWrites[binding] = descriptorWrite;
				binding++;
			}

			vkUpdateDescriptorSets(instance->GetNativeDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);

			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				graphicsPipeline->GetNativePipelineLayout(),
				0,
				1,
				&descriptorSet,
				0,
				nullptr
			);
		}

		VkBuffer vertexBuffers[2];
		VkDeviceSize offsets[] = { 0, 0 };
		uint32 i = 0;
		if (vertexBuffer.buffer != nullptr) {
			vertexBuffers[i] = vertexBuffer.buffer->GetNativeBuffer();
			i++;
		}

		if (instanceBuffer.buffer != nullptr) {
			vertexBuffers[i] = instanceBuffer.buffer->GetNativeBuffer();
			i++;
		}

		if (i != 0) {
			vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);
		}

		if (indexBuffer.buffer != nullptr) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer->GetNativeBuffer(), 0, VK_INDEX_TYPE_UINT16);
		}

		VkViewport viewport {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float) extent.width;
		viewport.height = (float) extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor {};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDrawIndexed(commandBuffer, graphicsPipeline->indexCount, graphicsPipeline->instanceCount, 0, 0, 0);
	}
}