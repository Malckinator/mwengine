#pragma once

#include "src/pch.h"
#include "vulkan/vulkan.h"
#include "src/Window.h"
#include "vkPhysicalDevice.h"
#include "../Instance.h"

namespace mwengine::RenderAPI::Vulkan {
	class vkInstance : Instance {
	public:
		vkInstance(Window* window, const std::string& appName = "app", uint32 appVersion = MW_MAKE_VERSION(1, 0, 0), bool debug = true);
		~vkInstance();

		std::vector<PhysicalDevice*> GetPhysicalDevices();
		PhysicalDevice* GetOptimalPhysicalDevice();
		void SetPhysicalDevice(PhysicalDevice* physicalDevice);
		void WaitUntilIdle();
		Window* GetWindow();
		PhysicalDevice* GetPhysicalDevice();

		VkInstance GetNativeInstance();
		VkDebugUtilsMessengerEXT GetNativeDebugMessenger();
		VkSurfaceKHR GetNativeSurface();
		VkDevice GetNativeDevice();
		VkCommandPool GetNativeCommandPool();
	private:
		Window* window;
		bool debug;
		std::vector<PhysicalDevice*> physicalDevices;

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkSurfaceKHR surface;
		PhysicalDevice* physicalDevice;
		VkDevice device;
		VkCommandPool commandPool;
	};
}