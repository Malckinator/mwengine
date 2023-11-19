#pragma once

#include "src/pch.h"
#include "vkShader.h"
#include "vkInstance.h"
#include "vkImageFormat.h"
#include "vkBuffer.h"
#include "vkImage.h"

namespace mwengine::RenderAPI::Vulkan {
	struct ShaderStage {
		vkShader* shader;
		std::string entry;
	};

	enum CullingMode {
		CULLING_MODE_NONE = 0,
		CULLING_MODE_FRONT_BIT = MW_BIT(0),
		CULLING_MODE_BACK_BIT = MW_BIT(1),
		CULLING_MODE_FRONT_AND_BACK = CULLING_MODE_FRONT_BIT | CULLING_MODE_BACK_BIT
	};

	class vkGraphicsPipeline {
	public:
		vkGraphicsPipeline(vkInstance* instance);
		~vkGraphicsPipeline();

		void Rebuild();
		void SetDefaults();

		std::vector<ShaderDataType> vertexSpecification;
		uint32 indexCount;
		std::vector<ShaderDataType> instanceSpecification;
		uint32 instanceCount;
		bool hasUniformBuffer;
		uint32 textureCount;
		CullingMode cullingMode;
		vkImageFormat* imageFormat;
		ShaderStage vertexShader;
		ShaderStage fragmentShader;

		vkInstance* GetInstance();
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