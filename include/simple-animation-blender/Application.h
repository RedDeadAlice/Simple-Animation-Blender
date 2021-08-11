#pragma once
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <simple-animation-blender/Logger.h>
#define STR VK_STRUCTURE_TYPE
#define ALLOCATOR nullptr

class Application
{
private:
    struct Framebuffer
    {
        VkFramebuffer vkHandle;
        VkImageView swapchainImgView, depthImgView;
        VkImage depthImg;
        VkDeviceMemory depthImgMemory;
    };
    int fbHeight, fbWidth;
    char *meshPath;
    GLFWwindow *window;
    VkInstance vkInstance;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    VkSurfaceKHR surface;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VkSurfaceFormatKHR surfaceFormat;
    VkDevice device;
    VkSwapchainKHR swapchain;
    VkCommandPool cmdPool;
    VkDescriptorPool descriptorPool;
    VkRenderPass renderPass;
    std::vector<Framebuffer> framebuffers;
    VkDescriptorSetLayout dsl;
    VkPipelineLayout pipelineLayout;
    uint32_t graphicsQueueFamilyIndex, presentQueueFamilyIndex,
        graphicsQueuesCount, presentQueuesCount;
    bool sameQueueForGraphicsAndPresent = false;
    bool debug = true; //TODO false on release
    Application();
    void createWindow(int height, int width);
    void createVkInstance();
    void createPhysicalDevice();
    void createSurface();
    void createQueuesFamilies();
    void createLogicalDevice();
    VkSwapchainKHR createSwapchain(VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);
    void createCommandPool();
    void createDescriptorPool();
    void createRenderPass();
    void createFramebuffers();
    void createDescriptorSetLayout();
    void createPipelineLayout();
    VkDeviceMemory allocateMemory(VkMemoryRequirements memReq, VkMemoryPropertyFlags properties);

public:
    static Application &instance();
    void init(char *meshPath);
    void terminate();
};