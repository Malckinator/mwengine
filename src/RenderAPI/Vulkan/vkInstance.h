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
		virtual ~vkInstance();

		virtual std::vector<PhysicalDevice*> GetPhysicalDevices();
		virtual PhysicalDevice* GetOptimalPhysicalDevice();
		virtual void SetPhysicalDevice(PhysicalDevice* physicalDevice);
		virtual void WaitUntilIdle();
		virtual Window* GetWindow();
		virtual PhysicalDevice* GetPhysicalDevice();

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