#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include <fstream>
#include <array>
#include <cstring>

extern const std::vector<const char*> validationLayers;
extern const std::vector<const char*> deviceExtensions;

// #ifdef NDEBUG
// const bool enableValidationLayers = false;
// #else
const bool enableValidationLayers = true;
// #endif

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestoryDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

std::vector<char> readFile(const std::string& filename);

struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};

extern const std::vector<Vertex> gVertices;

class HelloTriangleApplication {
public:
    void run();

    void initWindow();

    static void framebufferResizeCallback(GLFWwindow* window, int widht, int height);

    void initVulkan();

    void recreateSwapChain();

    void createInstance();

    bool checkValidationLayerSupport();

    std::vector<const char*> getRequiredExtensions();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    void setupDebugMessenger();

    void createSurface();

    void pickPhysicalDevice();

    bool isDeviceSutiable(VkPhysicalDevice device);

    struct QueueFamilyIndices {
        std::optional<unsigned int> graphicsFamily;
        std::optional<unsigned int> presentFamily;

        bool isComplete();
    };

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    void createLogicalDevice();

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    void createSwapChain();

    void createImageViews();

    void createRenderPass();

    void createGraphicsPipeline();

    VkShaderModule createShaderModule(const std::vector<char>& code);

    void createFramebuffers();

    void createCommandPool();

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void createVertexBuffer(std::vector<Vertex> vertices);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties);

    void createCommandBuffers();

    void createSyncObjects();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, unsigned int imageIndex, size_t verticesSize);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void mainLoop(size_t verticesSize);

    void drawFrame(size_t verticesSize);

    void cleanupSwapChain();

    void cleanup();

    GLFWwindow* m_Window;

    const unsigned int m_Wdith{ 800 };
    const unsigned int m_Height{ 600 };

    VkInstance m_Instance;

    VkDebugUtilsMessengerEXT m_DebugMessenger;

    VkSurfaceKHR m_Surface;

    VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };

    VkDevice m_Device;

    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;

    VkSwapchainKHR m_SwapChain;
    std::vector<VkImage> m_SwapChainImages;
    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;

    std::vector<VkImageView> m_SwapChainImageViews;

    VkRenderPass m_RenderPass;
    VkPipelineLayout m_PipelineLayout;

    VkPipeline m_GraphicsPipeline;

    std::vector<VkFramebuffer> m_SwapChainFramebuffers;

    VkCommandPool m_CommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;

    const int m_MaxFramesInFlight{ 2 };

    unsigned int m_CurrentFrame{ 0 };

    bool m_FramebufferResized{ false };

    VkBuffer m_VertexBuffer;
    VkDeviceMemory m_VertexBufferMemory;

    bool m_Drawn{ false };
};

// int main() {
//     HelloTriangleApplication app;

//     try {
//         app.run();
//     }
//     catch (const std::exception& e) {
//         std::cerr << e.what() << std::endl;

//         std::cin.get();
//         return EXIT_FAILURE;
//     }

//     std::cin.get();
//     return EXIT_SUCCESS;
// }
