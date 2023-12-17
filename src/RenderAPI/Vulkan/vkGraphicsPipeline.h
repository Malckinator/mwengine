#pragma once

#include "src/pch.h"
#include "vkShader.h"
#include "vkInstance.h"
#include "vkBuffer.h"
#include "vkImage.h"
#include "../GraphicsPipeline.h"
#include "vkFramebuffer.h"

namespace mwengine::RenderAPI::Vulkan {
	class vkGraphicsPipeline : GraphicsPipeline {
	public:
		vkGraphicsPipeline(Instance* instance);
		virtual ~vkGraphicsPipeline();

		virtual void Rebuild();
		virtual void SetDefaults();
		virtual Instance* GetInstance();

		VkPipeline GetNativePipeline();
		VkPipelineLayout GetNativePipelineLayout();
		VkDescriptorSet GetNativeDescriptorSet();
	private:
		void Destroy();

		vkInstance* instance;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;

		bool oldHasDescriptorSets;
	};
}