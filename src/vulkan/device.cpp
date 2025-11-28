#include "device.h"

#include <cmath>
#include <iostream>
#include <map>
#include <ranges>

#include <SDL3/SDL_vulkan.h>

namespace Cocoa::Vulkan {
    Device::Device(DeviceDesc desc) {
        CreateInstance();
        GetPhysicalDevice(desc);
        DiscoverQueues(desc);
        CreateDevice();
        CreateAllocator();
        CreateCommandPool();
        CreateCommandBuffers();
    }
    
    Device::~Device() {
        _device->waitIdle();

        _commandBuffers.clear();
        _commandPool.reset();

        vmaDestroyAllocator(_allocator);
        _allocator = nullptr;

        _queues.clear();
        _device.reset();
        _instance.reset();
    }
    
    std::unique_ptr<Encoder> Device::Encode(Swapchain* swapchain) {
        auto backBuffer = swapchain->GetNextBackBuffer();
        
        EncoderDesc desc = {
            .swapchain = swapchain,
            .cmd = _commandBuffers[_frame].get()
        };
        auto encoder = std::make_unique<Encoder>(desc);

        vk::ImageSubresourceRange toColorSubresourceRange;
        toColorSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1)
                    .setBaseMipLevel(0)
                    .setLevelCount(1);
        vk::ImageMemoryBarrier2 toColor;
        toColor.setImage(backBuffer.image)
                    .setSrcStageMask(vk::PipelineStageFlagBits2::eNone)
                    .setSrcAccessMask(vk::AccessFlagBits2::eNone)
                    .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
                    .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
                    .setOldLayout(vk::ImageLayout::eUndefined)
                    .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
                    .setSubresourceRange(toColorSubresourceRange);
        vk::DependencyInfo dependencyDescriptor;
        dependencyDescriptor.setImageMemoryBarriers(toColor);
        encoder->GetCommandBuffer().pipelineBarrier2(dependencyDescriptor);

        return encoder;
    }

    void Device::EndEncoding(std::unique_ptr<Encoder> encoder) {
        auto swapchain = encoder->GetTargetSwapchain();
        auto commandBuffer = encoder->GetCommandBuffer();
        auto backBuffer = swapchain->GetCurrentBackBuffer();

        vk::ImageSubresourceRange toColorSubresourceRange;
        toColorSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1)
                    .setBaseMipLevel(0)
                    .setLevelCount(1);
        vk::ImageMemoryBarrier2 toPresent;
        toPresent.setImage(backBuffer.image)
                    .setSrcStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
                    .setSrcAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
                    .setDstStageMask(vk::PipelineStageFlagBits2::eNone)
                    .setDstAccessMask(vk::AccessFlagBits2::eNone)
                    .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
                    .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
                    .setSubresourceRange(toColorSubresourceRange);
        vk::DependencyInfo dependencyDescriptor;
        dependencyDescriptor.setImageMemoryBarriers(toPresent);
        encoder->GetCommandBuffer().pipelineBarrier2(dependencyDescriptor);
        encoder.reset();
        

        swapchain->Submit(commandBuffer);
        swapchain->Present();

        _frame = (_frame + 1) % 2;
    }

    std::optional<GPUQueue> Device::GetQueue(GPUQueueType queueType) {
        if (!_queues.contains(queueType)) return std::nullopt;
        return _queues[queueType];
    } 

    void Device::CreateInstance() {
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

    void Device::GetPhysicalDevice(DeviceDesc desc) {
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

    GPUQueue Device::GetSupportedQueue(std::vector<vk::QueueFamilyProperties> queueFamilies, GPUQueueType type) {
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

    void Device::DiscoverQueues(DeviceDesc desc) {
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

    void Device::CreateDevice() {
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

    void Device::CreateAllocator() {
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

    void Device::CreateCommandPool() {
        vk::CommandPoolCreateInfo commandPoolDescriptor;
        commandPoolDescriptor.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                        .setQueueFamilyIndex(_queues[GPUQueueType::Graphics].family);
        _commandPool = _device->createCommandPoolUnique(commandPoolDescriptor);
    }

    void Device::CreateCommandBuffers() {
        vk::CommandBufferAllocateInfo commandBufferDescriptor;
            commandBufferDescriptor.setCommandBufferCount(2)
                        .setCommandPool(_commandPool.get())
                        .setLevel(vk::CommandBufferLevel::ePrimary);
        _commandBuffers = _device->allocateCommandBuffersUnique(commandBufferDescriptor);
    }
}