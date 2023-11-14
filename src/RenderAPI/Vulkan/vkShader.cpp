#include "pch.h"
#include "vkShader.h"

namespace mwengine::RenderAPI::Vulkan {
	vkShader::vkShader(vkInstance* instance, const std::vector<uint8>& code) {
		Create(instance, code);
	}

	vkShader::vkShader(vkInstance* instance, const std::string& filepath) {
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
			MW_ERROR("Shader file failed to open.");

		uint32 size = (uint32) file.tellg();
		std::vector<uint8> code(size);
		file.seekg(0);
		file.read((char*) code.data(), size);
		file.close();
		Create(instance, code);
	}
	
	vkShader::~vkShader() {
		vkDestroyShaderModule(instance->GetNativeDevice(), shaderModule, nullptr);
	}

	VkShaderModule vkShader::GetNativeShaderModule() {
		return shaderModule;
	}

	vkInstance* vkShader::GetInstance() {
		return instance;
	}
	
	void vkShader::Create(vkInstance* instance, const std::vector<uint8>& code) {
		this->instance = instance;

		VkShaderModuleCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = (uint32*) code.data();
		
		if (vkCreateShaderModule(instance->GetNativeDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			MW_ERROR("Failed to create shader.");
		}
	}

	uint32 shaderDataTypeSizes[] = {
		sizeof(uint8),
		sizeof(uint16),
		sizeof(uint32),
		sizeof(uint32) * 2,
		sizeof(uint32) * 3,
		sizeof(uint32) * 4,
		sizeof(int8),
		sizeof(int16),
		sizeof(int32),
		sizeof(int32) * 2,
		sizeof(int32) * 3,
		sizeof(int32) * 4,
		sizeof(float),
		sizeof(float) * 2,
		sizeof(float) * 3,
		sizeof(float) * 4,
	};

	uint32 GetShaderDataTypeSize(ShaderDataType shaderDataType) {
		return shaderDataTypeSizes[shaderDataType];
	}

	VkFormat shaderDataTypeVkEnums[] = {
		VK_FORMAT_R8_UINT,
		VK_FORMAT_R16_UINT,
		VK_FORMAT_R32_UINT,
		VK_FORMAT_R32G32_UINT,
		VK_FORMAT_R32G32B32_UINT,
		VK_FORMAT_R32G32B32A32_UINT,
		VK_FORMAT_R8_SINT,
		VK_FORMAT_R16_SINT,
		VK_FORMAT_R32_SINT,
		VK_FORMAT_R32G32_SINT,
		VK_FORMAT_R32G32B32_SINT,
		VK_FORMAT_R32G32B32A32_SINT,
		VK_FORMAT_R32_SFLOAT,
		VK_FORMAT_R32G32_SFLOAT,
		VK_FORMAT_R32G32B32_SFLOAT,
		VK_FORMAT_R32G32B32A32_SFLOAT
	};

	VkFormat GetShaderDataTypeVkEnum(ShaderDataType shaderDataType) {
		return shaderDataTypeVkEnums[shaderDataType];
	}

    uint8 GetShaderDataTypeSlotsUsed(ShaderDataType shaderDataType) {
		float used = GetShaderDataTypeSize(shaderDataType) / (float) GetShaderDataTypeSize(SHADER_DATA_TYPE_FLOAT_VEC4);
		return (uint8) ceilf(used);
    }

    std::vector<ShaderDataType> UnpackComplexShaderDataTypes(std::vector<ShaderDataType> types) {
		uint32 count = 0;

		for (ShaderDataType type : types) {
			switch (type) {
			case SHADER_DATA_TYPE_FLOAT_MAT4:
				count += 4;
				break;
			default:
				count++;
				break;
			}
		}

		std::vector<ShaderDataType> unpacked(count);
		uint32 i = 0;
		for (ShaderDataType type : types) {
			switch (type) {
			case SHADER_DATA_TYPE_FLOAT_MAT4:
				unpacked[i + 0] = SHADER_DATA_TYPE_FLOAT_VEC4;
				unpacked[i + 1] = SHADER_DATA_TYPE_FLOAT_VEC4;
				unpacked[i + 2] = SHADER_DATA_TYPE_FLOAT_VEC4;
				unpacked[i + 3] = SHADER_DATA_TYPE_FLOAT_VEC4;
				i += 4;
				break;
			default:
				unpacked[i] = type;
				i++;
				break;
			}
		}

		return unpacked;
    }
}