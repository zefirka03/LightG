#include "vulkan_device.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	printf("validation layer: %s\n", pCallbackData->pMessage);

	return VK_FALSE;
}

void VulkanDevice::_create_instance(){
	if (m_enable_validation_layers && !_check_validation_layer_support())
		throw std::runtime_error("validation layers requested, but not available!");
	
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "LittleVulkanEngine App";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
	
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	
	auto extensions = _get_required_extensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (m_enable_validation_layers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
		createInfo.ppEnabledLayerNames = m_validation_layers.data();
		
		_populate_debug_messenger_create_info(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}
	
	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
		throw std::runtime_error("failed to create instance!\n");
}

void VulkanDevice::_create_surface(){
	m_window.create_window_surface(m_instance, &m_surface);
}

std::vector<const char *> VulkanDevice::_get_required_extensions() {
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_enable_validation_layers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

VulkanQueueFamilyIndices VulkanDevice::find_queue_families(VkPhysicalDevice device) {
	VulkanQueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
			indices.graphicsFamilyHasValue = true;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
			indices.presentFamilyHasValue = true;
		}
		if (indices.isComplete()) {
			break;
		}

		++i;
	}

	return indices;
}

VulkanQueueFamilyIndices VulkanDevice::find_physical_queue_families(){
	return find_queue_families(m_physical_device);
}

SwapChainSupportDetails VulkanDevice::_query_swap_chain_support(VkPhysicalDevice device) {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			device,
			m_surface,
			&presentModeCount,
			details.presentModes.data()
		);
	}
	return details;
}

bool VulkanDevice::_check_validation_layer_support() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : m_validation_layers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

bool VulkanDevice::_check_device_extension_support(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(
		device,
		nullptr,
		&extensionCount,
		availableExtensions.data());

	std::set<std::string> requiredExtensions(m_device_extensions.begin(), m_device_extensions.end());

	for (const auto& extension : availableExtensions) 
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}

bool VulkanDevice::_is_device_suitable(VkPhysicalDevice device) {
	VulkanQueueFamilyIndices indices = find_queue_families(device);

	bool extensionsSupported = _check_device_extension_support(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = _query_swap_chain_support(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate &&
		supportedFeatures.samplerAnisotropy;
}

void VulkanDevice::_pick_physical_device() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	printf("Device count: %ld\n", deviceCount);
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (_is_device_suitable(device)) {
			m_physical_device = device;
			break;
		}
	}

	if (m_physical_device == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	vkGetPhysicalDeviceProperties(m_physical_device, &m_properties);
	printf("physical device: %s\n", m_properties.deviceName);
}

void VulkanDevice::_populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;  // Optional
}

void VulkanDevice::_create_logical_device() {
	VulkanQueueFamilyIndices indices = find_queue_families(m_physical_device);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size());
	createInfo.ppEnabledExtensionNames = m_device_extensions.data();

	// might not really be necessary anymore because device specific validation layers
	// have been deprecated
	if (m_enable_validation_layers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
		createInfo.ppEnabledLayerNames = m_validation_layers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_physical_device, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_device, indices.graphicsFamily, 0, &m_graphics_queue);
	vkGetDeviceQueue(m_device, indices.presentFamily, 0, &m_present_queue);
}

void VulkanDevice::_create_command_pool() {
	VulkanQueueFamilyIndices queueFamilyIndices = find_physical_queue_families();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags =
		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_command_pool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

VkFormat VulkanDevice::find_supported_format(
	const std::vector<VkFormat>& candidates,
	VkImageTiling tiling,
	VkFormatFeatureFlags features
) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (
			tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}
	throw std::runtime_error("failed to find supported format!");
}

uint32_t VulkanDevice::find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_physical_device, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanDevice::create_image_with_info(
	const VkImageCreateInfo& imageInfo,
	VkMemoryPropertyFlags properties,
	VkImage& image,
	VkDeviceMemory& imageMemory
) {
	if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	if (vkBindImageMemory(m_device, image, imageMemory, 0) != VK_SUCCESS) {
		throw std::runtime_error("failed to bind image memory!");
	}
}

VulkanDevice::VulkanDevice(VulkanWindow& window) 
	: m_window(window) {
	_create_instance();
	_create_surface();
	_pick_physical_device();
	_create_logical_device();
	_create_command_pool();
}

VulkanDevice::~VulkanDevice(){

}

VkDevice VulkanDevice::device() const {
	return m_device;
}

VkCommandPool VulkanDevice::get_command_pool() const {
	return m_command_pool;
}

VkSurfaceKHR VulkanDevice::surface() const {
	return m_surface;
}

VkQueue VulkanDevice::graphics_queue() const {
	return m_graphics_queue;
}

VkQueue VulkanDevice::present_queue() const {
	return m_present_queue;
}

SwapChainSupportDetails VulkanDevice::get_swap_chain_support() {
	return _query_swap_chain_support(m_physical_device);
}