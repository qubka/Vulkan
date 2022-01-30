#pragma once

namespace Engine {
    class Window;

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool isComplete() const { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class Device {
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
        explicit Device(Window& window);
        ~Device();
        Device(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        VkCommandPool getCommandPool() const;
        VkDevice getDevice() const;
        VkSurfaceKHR getSurface() const;
        VkQueue getGraphicsQueue() const;
        VkQueue getPresentQueue() const;
        SwapChainSupportDetails getSwapChainSupport() const;
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        QueueFamilyIndices findPhysicalQueueFamilies() const;
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                     VkFormatFeatureFlags features) const;

        operator VkDevice() const;

        // Buffer helper functions
        void createBuffer(
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties,
                VkBuffer& buffer,
                VkDeviceMemory& bufferMemory) const;
        VkCommandBuffer beginSingleTimeCommands() const;
        void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) const;
        void createImageWithInfo(
                const VkImageCreateInfo& imageInfo,
                VkMemoryPropertyFlags properties,
                VkImage& image,
                VkDeviceMemory& imageMemory) const;

        VkPhysicalDeviceProperties properties;

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        // helper functions
        bool isDeviceSuitable(VkPhysicalDevice device) const;
        std::vector<const char*> getRequiredExtensions() const;
        bool checkValidationLayerSupport() const;
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;
        void hasGflwRequiredInstanceExtensions() const;
        bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

        Window& window;

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkCommandPool commandPool;
        VkDevice device;
        VkSurfaceKHR surface;
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
}
