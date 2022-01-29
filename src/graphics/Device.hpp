#pragma once

namespace Engine {
    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    class Window;
    class Device {
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
    public:
        explicit Device(const Window& window);
        ~Device();
        Device(const Device&) = delete;
        Device(Device&&) = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&) = delete;

        const vk::UniqueDevice& operator()() const;
        const vk::UniqueDevice& getDevice() const;
        const vk::PhysicalDevice& getPhysicalDevice() const;
        const vk::SurfaceKHR& getSurface() const;
        const vk::Queue& getGraphicsQueue() const;
        const vk::Queue& getPresentQueue() const;
        const vk::CommandPool& getCommandPool() const;

        SwapChainSupportDetails getSwapChainSupport() const;
        QueueFamilyIndices findPhysicalQueueFamilies() const;

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface(const Window& window);
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        std::vector<const char*> getRequiredExtensions() const;
        bool checkValidationLayerSupport() const;
        void hasGflwRequiredInstanceExtensions() const;
        bool isDeviceSuitable(const vk::PhysicalDevice& device) const;
        bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device) const;
        QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) const;
        SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) const;

        vk::UniqueInstance instance;
        vk::PhysicalDevice physicalDevice;
        vk::UniqueDevice device;
        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
        vk::SurfaceKHR surface;
        vk::CommandPool commandPool;

        VkDebugUtilsMessengerEXT callback;

        const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
}