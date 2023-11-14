#include "pch.h"
#include "vkGraphicsPipeline.h"
#include "vulkan/vulkan.h"

namespace mwengine::RenderAPI::Vulkan {
	vkGraphicsPipeline::vkGraphicsPipeline(vkInstance* instance) {
		this->pipeline = nullptr;
		this->pipelineLayout = nullptr;
		this->instance = instance;

		imageFormat = nullptr;
		vertexShader = { nullptr, "" };
		fragmentShader = { nullptr, "" };
		vertexSpecification = {};
		indexCount = 0;
		cullingMode = CULLING_MODE_NONE;
	}

	vkGraphicsPipeline::~vkGraphicsPipeline() {
		Destroy();
	}

	void vkGraphicsPipeline::Rebuild() {
		Destroy();

		glm::uvec2 size = imageFormat->GetSize();
		VkExtent2D extent = { (uint32) size.x, (uint32) size.y };

		std::vector<ShaderDataType> unpackedVertexSpec = UnpackComplexShaderDataTypes(vertexSpecification);
		std::vector<ShaderDataType> unpackedInstanceSpec = UnpackComplexShaderDataTypes(instanceSpecification);
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(unpackedVertexSpec.size() + unpackedInstanceSpec.size());
		uint32 vertexSize = 0;
		uint32 location = 0;

		for (uint32 i = 0; i < unpackedVertexSpec.size(); i++) {
			vertexAttributeDescriptions[i].binding = 0;
			vertexAttributeDescriptions[i].location = i;
			vertexAttributeDescriptions[i].format = GetShaderDataTypeVkEnum(unpackedVertexSpec[i]);
			vertexAttributeDescriptions[i].offset = vertexSize;

			vertexSize += GetShaderDataTypeSize(unpackedVertexSpec[i]);
			location++;
		}

		uint32 instanceSize = 0;
		for (uint32 i = 0; i < unpackedInstanceSpec.size(); i++) {
			vertexAttributeDescriptions[location].binding = 1;
			vertexAttributeDescriptions[location].location = location;
			vertexAttributeDescriptions[location].format = GetShaderDataTypeVkEnum(unpackedInstanceSpec[i]);
			vertexAttributeDescriptions[location].offset = instanceSize;

			instanceSize += GetShaderDataTypeSize(unpackedInstanceSpec[i]);
			location++;
		}

		VkVertexInputBindingDescription vertexBindingDescription {};
		vertexBindingDescription.binding = 0;
		vertexBindingDescription.stride = vertexSize;
		vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputBindingDescription instanceBindingDescription {};
		instanceBindingDescription.binding = 1;
		instanceBindingDescription.stride = instanceSize;
		instanceBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

		VkVertexInputBindingDescription bindingDescriptions[2] = { vertexBindingDescription, instanceBindingDescription };

		VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertexShader.shader->GetNativeShaderModule();
		vertShaderStageInfo.pName = vertexShader.entry.c_str();

		VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragmentShader.shader->GetNativeShaderModule();
		fragShaderStageInfo.pName = fragmentShader.entry.c_str();

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = (uint32) dynamicStates.size();
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = (uint32) vertexAttributeDescriptions.size();
		vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
		vertexInputInfo.vertexBindingDescriptionCount = 2;
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float) extent.width;
		viewport.height = (float) extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor {};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;

		VkPipelineViewportStateCreateInfo viewportState {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_NONE;

		if (cullingMode & CULLING_MODE_FRONT_BIT)
			rasterizer.cullMode |= VK_CULL_MODE_FRONT_BIT;
		if (cullingMode & CULLING_MODE_BACK_BIT)
			rasterizer.cullMode |= VK_CULL_MODE_BACK_BIT;

		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampling {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(instance->GetNativeDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			MW_ERROR("Failed to create pipeline layout.");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = imageFormat->GetNativeRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(instance->GetNativeDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
			MW_ERROR("Failed to create graphics pipeline.");
		}
	}

	vkInstance* vkGraphicsPipeline::GetInstance() {
		return instance;
	}

	VkPipeline vkGraphicsPipeline::GetNativePipeline() {
		return pipeline;
	}

	void vkGraphicsPipeline::Destroy() {
		if (pipeline != nullptr) {
			vkDestroyPipeline(instance->GetNativeDevice(), pipeline, nullptr);
			vkDestroyPipelineLayout(instance->GetNativeDevice(), pipelineLayout, nullptr);
		}
	}
}