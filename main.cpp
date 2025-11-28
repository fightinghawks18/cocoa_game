#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

// Stops VMA from printing ~1000 warnings
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#define MAX_FRAMES_IN_FLIGHT 2

enum class GPUPowerPreference {
    HighPerformance,
    LowPower,
};

enum class GPUQueueType {
    Unknown,
    Graphics,
    Compute,
    Transfer
};

struct GPUQueue {
    GPUQueueType type;
    uint32_t family;
    vk::Queue queue;
};

struct RenderDeviceDesc {
    SDL_Window* window;
    GPUPowerPreference powerPreference;
    std::vector<GPUQueueType> desiredQueues;
};

static std::string GPUQueueTypeToString(GPUQueueType type) {
    switch (type) {
    case GPUQueueType::Unknown:
        return "Unknown";
    case GPUQueueType::Graphics:
        return "Graphics";
    case GPUQueueType::Compute:
        return "Compute";
    case GPUQueueType::Transfer:
        return "Transfer";
    }
}

class RenderDevice {
public:
    RenderDevice(RenderDeviceDesc desc) {
        CreateInstance();
        GetPhysicalDevice(desc);
        DiscoverQueues(desc);
        CreateDevice();
        CreateAllocator();
        CreateSurface(desc);
        CreateSwapchain();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateFences();
        CreateSemaphores();
    }

    ~RenderDevice() {
        _device->waitIdle();

        _commandBuffers.clear();
        _commandPool.reset();

        _fences.clear();
        _renderSemaphores.clear();
        _imageSemaphores.clear();

        _swapchainImageViews.clear();
        _swapchain.reset();
        SDL_Vulkan_DestroySurface(_instance.get(), _surface, nullptr);

        vmaDestroyAllocator(_allocator);
        _allocator = nullptr;

        _queues.clear();
        _device.reset();
        _instance.reset();
    }


    void StartRendering() {
        vk::Result waitForPreviousOperations = _device->waitForFences(1, &_fences[_frame].get(), true, INFINITY);
        if (waitForPreviousOperations != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to wait for previous operations");
        }
        _device->resetFences(_fences[_frame].get());

        vk::Result getNextSwapchainImageIndex = _device->acquireNextImageKHR(_swapchain.get(), INFINITY, _imageSemaphores[_frame].get(), nullptr, &_imageIndex);
        if (getNextSwapchainImageIndex != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to get next swapchain image index!");
        }

        auto& commandBuffer = _commandBuffers[_frame].get();
        commandBuffer.reset();

        vk::CommandBufferBeginInfo commandBeginDescriptor;
        commandBeginDescriptor.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

        commandBuffer.begin(commandBeginDescriptor);

        auto& image = _swapchainImages[_imageIndex];

        vk::ImageSubresourceRange toColorSubresourceRange;
        toColorSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1)
                    .setBaseMipLevel(0)
                    .setLevelCount(1);
        vk::ImageMemoryBarrier2 toColor;
        toColor.setImage(image)
                    .setSrcStageMask(vk::PipelineStageFlagBits2::eNone)
                    .setSrcAccessMask(vk::AccessFlagBits2::eNone)
                    .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
                    .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
                    .setOldLayout(vk::ImageLayout::eUndefined)
                    .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
                    .setSubresourceRange(toColorSubresourceRange);
        vk::DependencyInfo dependencyDescriptor;
        dependencyDescriptor.setImageMemoryBarriers(toColor);
        commandBuffer.pipelineBarrier2(dependencyDescriptor);

        vk::ClearColorValue clearColor;
        clearColor.setFloat32({0.6f, 0.21f, 0.36f, 1.0f});

        vk::RenderingAttachmentInfo renderingAttachmentDescriptor;
        renderingAttachmentDescriptor.setClearValue(clearColor)
                    .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
                    .setLoadOp(vk::AttachmentLoadOp::eClear)
                    .setStoreOp(vk::AttachmentStoreOp::eStore)
                    .setImageView(_swapchainImageViews[_imageIndex].get());
        
        vk::RenderingInfo renderingDescriptor;
        renderingDescriptor.setColorAttachments(renderingAttachmentDescriptor)
                    .setRenderArea({{0, 0}, {800, 600}})
                    .setViewMask(0)
                    .setLayerCount(1);

        vk::Extent2D extent;
        extent.setWidth(800)
                .setHeight(600);

        vk::Rect2D scissor;
        scissor.setOffset({0, 0})
                .setExtent(extent);
        vk::Viewport viewport;
        viewport.setWidth(extent.width)
                .setHeight(extent.height)
                .setMaxDepth(1)
                .setMinDepth(0)
                .setX(0)
                .setY(0);

        commandBuffer.beginRendering(renderingDescriptor);
        commandBuffer.setViewport(0, viewport);
        commandBuffer.setScissor(0, scissor);
    }

    void EndRendering() {
        auto& commandBuffer = _commandBuffers[_frame].get();
        commandBuffer.endRendering();

        auto& image = _swapchainImages[_imageIndex];

        vk::ImageSubresourceRange toColorSubresourceRange;
        toColorSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1)
                    .setBaseMipLevel(0)
                    .setLevelCount(1);
        vk::ImageMemoryBarrier2 toPresent;
        toPresent.setImage(image)
                    .setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
                    .setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
                    .setDstStageMask(vk::PipelineStageFlagBits2::eNone)
                    .setDstAccessMask(vk::AccessFlagBits2::eNone)
                    .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
                    .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
                    .setSubresourceRange(toColorSubresourceRange);
        vk::DependencyInfo dependencyDescriptor;
        dependencyDescriptor.setImageMemoryBarriers(toPresent);
        commandBuffer.pipelineBarrier2(dependencyDescriptor);

        commandBuffer.end();

        vk::SemaphoreSubmitInfo imageSubmitDescriptor;
        imageSubmitDescriptor.setSemaphore(_imageSemaphores[_frame].get())
                    .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);
        
        vk::SemaphoreSubmitInfo renderSubmitDescriptor;
        renderSubmitDescriptor.setSemaphore(_renderSemaphores[_frame].get())
                    .setStageMask(vk::PipelineStageFlagBits2::eAllCommands);

        vk::CommandBufferSubmitInfo commandSubmitDescriptor;
        commandSubmitDescriptor.setCommandBuffer(_commandBuffers[_frame].get());

        vk::SubmitInfo2 submitDescriptor;
        submitDescriptor.setWaitSemaphoreInfos(imageSubmitDescriptor)
                    .setSignalSemaphoreInfos(renderSubmitDescriptor)
                    .setCommandBufferInfos(commandSubmitDescriptor);
        
        _queues[GPUQueueType::Graphics].queue.submit2(submitDescriptor, _fences[_frame].get());

        vk::PresentInfoKHR presentDescriptor;
        presentDescriptor.setImageIndices({_imageIndex})
                    .setSwapchains({_swapchain.get()})
                    .setWaitSemaphores({_renderSemaphores[_frame].get()});
        vk::Result presentResult = _queues[GPUQueueType::Graphics].queue.presentKHR(presentDescriptor);
        if (presentResult != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to present to screen!");
        }

        _frame = (_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
private:
    vk::UniqueInstance _instance;
    vk::PhysicalDevice _gpu;
    vk::UniqueDevice _device;
    std::unordered_map<GPUQueueType, GPUQueue> _queues;
    VmaAllocator _allocator;

    vk::SurfaceKHR _surface;
    
    vk::UniqueSwapchainKHR _swapchain;
    std::vector<vk::Image> _swapchainImages;
    std::vector<vk::UniqueImageView> _swapchainImageViews;
    std::vector<bool> _swapchainInit;

    vk::UniqueCommandPool _commandPool;
    std::vector<vk::UniqueCommandBuffer> _commandBuffers;

    std::vector<vk::UniqueFence> _fences;
    std::vector<vk::UniqueSemaphore> _renderSemaphores;
    std::vector<vk::UniqueSemaphore> _imageSemaphores;

    uint32_t _frame = 0;
    uint32_t _imageIndex = 0;

    void CreateInstance() {
        vk::ApplicationInfo appDescriptor;
        appDescriptor.setApplicationVersion(VK_MAKE_API_VERSION(0, 1, 0, 0))
                    .setPApplicationName("Cocoa")
                    .setEngineVersion(VK_MAKE_API_VERSION(0, 1, 0, 0))
                    .setPEngineName("Cocoa")
                    .setApiVersion(VK_MAKE_API_VERSION(0, 1, 3, 0));
        
        vk::InstanceCreateFlags instanceFlags;

        #ifdef __APPLE__
            instanceFlags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
        #endif

        std::vector<const char*> extensions = {
            "VK_EXT_debug_utils"
        };

        uint32_t sdlCount;
        auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlCount);
        extensions.insert(extensions.end(), sdlExtensions, sdlExtensions + sdlCount);

        std::vector<const char*> layers = {
            "VK_LAYER_KHRONOS_validation",
        };

        vk::InstanceCreateInfo instanceDescriptor;
        instanceDescriptor.setPApplicationInfo(&appDescriptor)
                        .setPEnabledExtensionNames(extensions)
                        .setPEnabledLayerNames(layers)
                        .setFlags(instanceFlags);
        _instance = vk::createInstanceUnique(instanceDescriptor);
    }

    void GetPhysicalDevice(RenderDeviceDesc desc) {
        auto physicalDevices = _instance->enumeratePhysicalDevices();
        
        std::map<int32_t, vk::PhysicalDevice> physicalDevicesRanked;
        for (auto& physicalDevice : physicalDevices) {
            auto properties = physicalDevice.getProperties();

            int32_t score = 0;

            switch (properties.deviceType) {
                case vk::PhysicalDeviceType::eDiscreteGpu:
                    score += desc.powerPreference == GPUPowerPreference::HighPerformance ? 1000 : 100;
                    break;
                case vk::PhysicalDeviceType::eIntegratedGpu:
                    score += desc.powerPreference == GPUPowerPreference::HighPerformance ? 500 : 500;
                    break;
                case vk::PhysicalDeviceType::eCpu:
                    score += desc.powerPreference == GPUPowerPreference::HighPerformance ? 100 : 250;
                    break;
                case vk::PhysicalDeviceType::eOther:
                case vk::PhysicalDeviceType::eVirtualGpu:
                  break;
            }

            float capabilityWeight = (desc.powerPreference == GPUPowerPreference::HighPerformance) ? 1.0f : 0.2f;
            score += static_cast<int32_t>(std::log2(properties.limits.maxImageDimension2D) * 5 * capabilityWeight);

            // TODO: Add feature checking

            physicalDevicesRanked[score] = physicalDevice;
        }
        _gpu = physicalDevicesRanked.rbegin()->second;
        std::cout << "Found GPU: " << _gpu.getProperties().deviceName << std::endl;
    }

    GPUQueue GetSupportedQueue(std::vector<vk::QueueFamilyProperties> queueFamilies, GPUQueueType type) {
        vk::QueueFlags requestedFlags;
        switch (type) {
            case GPUQueueType::Unknown:
                break;
            case GPUQueueType::Graphics:
                requestedFlags |= vk::QueueFlagBits::eGraphics;
                break;
            case GPUQueueType::Transfer:
                requestedFlags |= vk::QueueFlagBits::eTransfer;
                break;
            case GPUQueueType::Compute:
                requestedFlags |= vk::QueueFlagBits::eCompute;
                break;
        }

        for (uint32_t i = 0; const auto& queueFamily : queueFamilies) {
            if (!(queueFamily.queueFlags & requestedFlags)) {
                i++;
                continue;
            }
            
            return {
                .type = type,
                .family = i,
                .queue = nullptr
            };
        }

        return {
            .type = GPUQueueType::Unknown,
            .family = 0,
            .queue = nullptr
        };
    }

    void DiscoverQueues(RenderDeviceDesc desc) {
        auto queueFamilies = _gpu.getQueueFamilyProperties();

        for (const auto& queueType : desc.desiredQueues) {
            if (queueType == GPUQueueType::Unknown) continue;
            if (_queues.contains(queueType)) continue;
            
            vk::QueueFlags requestedFlags;
            switch (queueType) {
                case GPUQueueType::Unknown:
                    break;
                case GPUQueueType::Graphics:
                    requestedFlags |= vk::QueueFlagBits::eGraphics;
                    break;
                case GPUQueueType::Transfer:
                    requestedFlags |= vk::QueueFlagBits::eTransfer;
                    break;
                case GPUQueueType::Compute:
                    requestedFlags |= vk::QueueFlagBits::eCompute;
                    break;
            }

            auto queue = GetSupportedQueue(queueFamilies, queueType);
            if (queue.type == GPUQueueType::Unknown) continue;
            _queues[queueType] = queue;
        }

        uint32_t totalSatisfied = 0;
        std::unordered_map<GPUQueueType, uint32_t> satisfiedQueues;
        for (const auto& queue : _queues) {
            bool found = false;
            for (const auto& queueRequested : desc.desiredQueues) {
                if (queue.first == queueRequested) {
                    satisfiedQueues[queue.first] += 1;
                    totalSatisfied += 1;

                    if (satisfiedQueues[queue.first] > 1) {
                        std::cout << "Reduntant " << GPUQueueTypeToString(queue.first) << " queue was specified, please remove this duplicate!" << std::endl;
                    } else {
                        std::cout << "Found " << GPUQueueTypeToString(queue.first) << " queue!" << std::endl;
                    }
                    found = true;
                }
            }

            if (!found) {
                std::cerr << "Failed to find " << GPUQueueTypeToString(queue.first) << " queue!" << std::endl;
            }
        }

        std::cout << "Satisfied " << totalSatisfied << " of " << desc.desiredQueues.size() << " queues!" << std::endl;
    }

    void CreateDevice() {
        // Vulkan requires unique queue families, it's possible
        // we may have same family indices so we must accumulate queue count to avoid this
        std::map<uint32_t, uint32_t> uniqueFamilies;
        for (const auto& queue : _queues | std::views::values) {
            uniqueFamilies[queue.family]++;
        }

        std::unordered_map<uint32_t, std::vector<float>> queuePriorities;

        std::vector<vk::DeviceQueueCreateInfo> deviceQueueDescriptors;
        for (const auto& [family, count] : uniqueFamilies) {
            queuePriorities[family] = std::vector<float>(count, 1.0f);
            vk::DeviceQueueCreateInfo deviceQueueDescriptor;
            deviceQueueDescriptor.setQueuePriorities(queuePriorities[family])
                                .setQueueCount(count)
                                .setQueueFamilyIndex(family);
            deviceQueueDescriptors.push_back(deviceQueueDescriptor);
        }

        std::vector<const char*> deviceExtensions = {
            "VK_KHR_swapchain",
            #ifdef __APPLE__
                "VK_KHR_portability_subset",
            #endif
            "VK_KHR_synchronization2"
        };

        vk::PhysicalDeviceVulkan13Features vulkan13Features;
        vulkan13Features.setDynamicRendering(true)
                        .setSynchronization2(true);

        vk::DeviceCreateInfo deviceDescriptor;
        deviceDescriptor.setPNext(&vulkan13Features)
                        .setPEnabledExtensionNames(deviceExtensions)
                        .setQueueCreateInfos(deviceQueueDescriptors);
        _device = _gpu.createDeviceUnique(deviceDescriptor);

        for (auto& queue : _queues | std::views::values) {
            queue.queue = _device->getQueue(queue.family, 0);
        }
    }

    void CreateAllocator() {
        VmaAllocatorCreateInfo vmaAllocatorDescriptor = {
            .flags = 0,
            .physicalDevice = _gpu,
            .device = _device.get(),
            .preferredLargeHeapBlockSize = 0,
            .pAllocationCallbacks = nullptr,
            .pDeviceMemoryCallbacks = nullptr,
            .pHeapSizeLimit = nullptr,
            .pVulkanFunctions = nullptr,
            .instance = _instance.get(),
            .vulkanApiVersion = VK_API_VERSION_1_3,
            .pTypeExternalMemoryHandleTypes = nullptr
        };

        VkResult createAllocator = vmaCreateAllocator(&vmaAllocatorDescriptor, &_allocator);
        if (createAllocator != VK_SUCCESS) {
            throw std::runtime_error("Failed to create allocator");
        }
    }

    void CreateSurface(RenderDeviceDesc desc) {
        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(desc.window, _instance.get(), nullptr,  &surface);
        _surface = surface;
    }

    void CreateSwapchain() {
        auto surfaceFormats = _gpu.getSurfaceFormatsKHR(_surface);
    
        vk::SurfaceFormatKHR chosenFormat = surfaceFormats[0];
        for (const auto& format : surfaceFormats) {
            if (format.format == vk::Format::eB8G8R8A8Srgb && 
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                chosenFormat = format;
                break;
            }
        }

        std::cout << "Using format: " << vk::to_string(chosenFormat.format) << std::endl;

        vk::SwapchainCreateInfoKHR swapchainDescriptor;
        swapchainDescriptor.setClipped(true)
                        .setImageExtent({800, 600})
                        .setImageFormat(chosenFormat.format)
                        .setImageColorSpace(chosenFormat.colorSpace)
                        .setImageSharingMode(vk::SharingMode::eExclusive)
                        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                        .setMinImageCount(MAX_FRAMES_IN_FLIGHT)
                        .setImageArrayLayers(1)
                        .setPresentMode(vk::PresentModeKHR::eFifo)
                        .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
                        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                        .setSurface(_surface);
        _swapchain = _device->createSwapchainKHRUnique(swapchainDescriptor);
        _swapchainImages = _device->getSwapchainImagesKHR(_swapchain.get());

        _swapchainImageViews.clear();
        _swapchainInit.resize(_swapchainImages.size(), false);
        for (const auto& image : _swapchainImages) {
            vk::ComponentMapping components;
            components.setR(vk::ComponentSwizzle::eIdentity)
                        .setG(vk::ComponentSwizzle::eIdentity)
                        .setB(vk::ComponentSwizzle::eIdentity)
                        .setA(vk::ComponentSwizzle::eIdentity);
            vk::ImageSubresourceRange subresourceRange;
            subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                        .setBaseArrayLayer(0)
                        .setLayerCount(1)
                        .setBaseMipLevel(0)
                        .setLevelCount(1);

            vk::ImageViewCreateInfo imageViewDescriptor;
            imageViewDescriptor.setImage(image)
                        .setViewType(vk::ImageViewType::e2D)
                        .setComponents(components)
                        .setFormat(chosenFormat.format)
                        .setSubresourceRange(subresourceRange);
            
            _swapchainImageViews.push_back(_device->createImageViewUnique(imageViewDescriptor));
        }
    }

    void CreateCommandPool() {
        vk::CommandPoolCreateInfo commandPoolDescriptor;
        commandPoolDescriptor.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                        .setQueueFamilyIndex(_queues[GPUQueueType::Graphics].family);
        _commandPool = _device->createCommandPoolUnique(commandPoolDescriptor);
    }

    void CreateCommandBuffers() {
        vk::CommandBufferAllocateInfo commandBufferDescriptor;
            commandBufferDescriptor.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT)
                        .setCommandPool(_commandPool.get())
                        .setLevel(vk::CommandBufferLevel::ePrimary);
        _commandBuffers = _device->allocateCommandBuffersUnique(commandBufferDescriptor);
    }

    void CreateFences() {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vk::FenceCreateInfo fenceDescriptor;
            fenceDescriptor.setFlags(vk::FenceCreateFlagBits::eSignaled);
            _fences.push_back(_device->createFenceUnique(fenceDescriptor));
        }
    }

    void CreateSemaphores() {
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vk::SemaphoreCreateInfo semaphoreDescriptor;
            _imageSemaphores.push_back(_device->createSemaphoreUnique(semaphoreDescriptor));
            _renderSemaphores.push_back(_device->createSemaphoreUnique(semaphoreDescriptor));
        }
    }
};

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error("Failed to start SDL3");
    }

    SDL_Window* window = SDL_CreateWindow("Cocoa", 800, 600, SDL_WINDOW_VULKAN);

    RenderDeviceDesc desc = {
        .window = window,
        .powerPreference = GPUPowerPreference::HighPerformance,
        .desiredQueues = {
            GPUQueueType::Graphics,
            GPUQueueType::Transfer,
            GPUQueueType::Compute
        }
    };
    
    auto renderDevice = std::make_unique<RenderDevice>(desc);

    bool gameRun = true;
    while (gameRun) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                gameRun = false;
                break;
            }
        }

        renderDevice->StartRendering();
        renderDevice->EndRendering();
    }

    std::cout << "Hello World!" << std::endl;

    renderDevice.reset();
    SDL_Quit();
    return 0;
}
