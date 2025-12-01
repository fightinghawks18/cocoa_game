#include "device.h"

#include <iostream>
#include <map>
#include <ranges>

#include <SDL3/SDL_vulkan.h>

#include "../graphics/helpers.h"
#include "../macros.h"
namespace Cocoa::Vulkan {
    Device::Device(Graphics::DeviceDesc desc) {
        _swapchainResources.emplace(1024);
        _surfaceResources.emplace(1024);
        _textureResources.emplace(1024);
        _bindGroupResources.emplace(1024);
        _shaderModuleResources.emplace(1024);
        _samplerResources.emplace(1024);
        _pipelineLayoutResources.emplace(1024);
        _renderPipelineResources.emplace(1024);
        _bufferResources.emplace(1024);

        CreateInstance();
        GetPhysicalDevice(desc);
        DiscoverQueues(desc);
        CreateDevice();
        CreateAllocator();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateDescriptorPool();
    }
    
    Device::~Device() {
        _device->waitIdle();

        _swapchainResources.reset();
        _surfaceResources.reset();
        _textureResources.reset();
        _bindGroupResources.reset();
        _shaderModuleResources.reset();
        _samplerResources.reset();
        _pipelineLayoutResources.reset();
        _renderPipelineResources.reset();
        _bufferResources.reset();

        _descriptorPool.reset();
        _commandBuffers.clear();
        _commandPool.reset();

        vmaDestroyAllocator(_allocator);
        _allocator = nullptr;

        _queues.clear();
        _device.reset();
        _instance.reset();
    }

    Graphics::SurfaceHandle Device::CreateSurface(SDL_Window* window) {
        return _surfaceResources->Emplace(this, window);
    }

    Graphics::SwapchainHandle Device::CreateSwapchain(Graphics::SwapchainDesc swapchainDesc) {
        return _swapchainResources->Emplace(this, swapchainDesc);
    }

    Graphics::BufferHandle Device::CreateBuffer(Graphics::BufferDesc bufferDesc) {
        return _bufferResources->Emplace(this, bufferDesc);
    }

    Graphics::PipelineLayoutHandle Device::CreatePipelineLayout(Graphics::PipelineLayoutDesc pipelineLayoutDesc) {
        return _pipelineLayoutResources->Emplace(this, pipelineLayoutDesc);
    }

    Graphics::RenderPipelineHandle Device::CreateRenderPipeline(Graphics::PipelineDesc renderPipelineDesc) {
        return _renderPipelineResources->Emplace(this, renderPipelineDesc);
    }

    Graphics::SamplerHandle Device::CreateSampler(vk::SamplerCreateInfo samplerDesc) {
        return _samplerResources->Emplace(this, samplerDesc);
    }

    Graphics::ShaderModuleHandle Device::CreateShaderModule(Graphics::ShaderModuleDesc shaderModuleDesc) {
        return _shaderModuleResources->Emplace(this, shaderModuleDesc);
    }

    Graphics::TextureHandle Device::CreateTexture(Graphics::TextureDesc textureDesc) {
        return _textureResources->Emplace(this, textureDesc);
    }

    Graphics::BindGroupHandle Device::CreateBindGroup(Graphics::BindGroupDesc bindGroupDesc) {
        return _bindGroupResources->Emplace(this, bindGroupDesc);
    }

    void Device::DestroySurface(Graphics::SurfaceHandle surface) {
        _surfaceResources->Remove(surface);
    }

    void Device::DestroySwapchain(Graphics::SwapchainHandle swapchain) {
        _swapchainResources->Remove(swapchain);
    }

    void Device::DestroyBuffer(Graphics::BufferHandle buffer) {
        _bufferResources->Remove(buffer);
    }

    void Device::DestroyPipelineLayout(Graphics::PipelineLayoutHandle pipelineLayout) {
        _pipelineLayoutResources->Remove(pipelineLayout);
    }

    void Device::DestroyRenderPipeline(Graphics::RenderPipelineHandle renderPipeline) {
        _renderPipelineResources->Remove(renderPipeline);
    }

    void Device::DestroySampler(Graphics::SamplerHandle sampler) {
        _samplerResources->Remove(sampler);
    }

    void Device::DestroyShaderModule(Graphics::ShaderModuleHandle shaderModule) {
        _shaderModuleResources->Remove(shaderModule);
    }

    void Device::DestroyTexture(Graphics::TextureHandle texture) {
        _textureResources->Remove(texture);
    }

    void Device::DestroyBindGroup(Graphics::BindGroupHandle bindGroup) {
        _bindGroupResources->Remove(bindGroup);
    }

    Surface* Device::GetSurfaceInstance(Graphics::SurfaceHandle surface) {
        return _surfaceResources->Get(surface);
    }

    Swapchain* Device::GetSwapchainInstance(Graphics::SwapchainHandle swapchain) {
        return _swapchainResources->Get(swapchain);
    }

    Buffer* Device::GetBufferInstance(Graphics::BufferHandle buffer) {
        return _bufferResources->Get(buffer);
    }

    PipelineLayout* Device::GetPipelineLayoutInstance(Graphics::PipelineLayoutHandle pipelineLayout) {
        return _pipelineLayoutResources->Get(pipelineLayout);
    }

    RenderPipeline* Device::GetRenderPipelineInstance(Graphics::RenderPipelineHandle renderPipeline) {
        return _renderPipelineResources->Get(renderPipeline);
    }

    Sampler* Device::GetSamplerInstance(Graphics::SamplerHandle sampler) {
        return _samplerResources->Get(sampler);
    }

    ShaderModule* Device::GetShaderModuleInstance(Graphics::ShaderModuleHandle shaderModule) {
        return _shaderModuleResources->Get(shaderModule);
    }

    Texture* Device::GetTextureInstance(Graphics::TextureHandle texture) {
        return _textureResources->Get(texture);
    }

    BindGroup* Device::GetBindGroupInstance(Graphics::BindGroupHandle bindGroup) {
        return _bindGroupResources->Get(bindGroup);
    }

    std::unique_ptr<Encoder> Device::Encode(Graphics::SwapchainHandle swapchain) {
        auto swapchainInstance = GetSwapchainInstance(swapchain);
        auto backBuffer = swapchainInstance->GetCurrentBackBuffer();
        
        EncoderDesc desc = {
            .swapchain = swapchain,
            .cmd = _commandBuffers[_frame].get()
        };
        auto encoder = std::make_unique<Encoder>(this, desc);

        vk::ImageSubresourceRange toColorSubresourceRange{};
        toColorSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1)
                    .setBaseMipLevel(0)
                    .setLevelCount(1);
        vk::ImageMemoryBarrier2 toColor{};
        toColor.setImage(GetTextureInstance(backBuffer)->Get())
                    .setSrcStageMask(vk::PipelineStageFlagBits2::eNone)
                    .setSrcAccessMask(vk::AccessFlagBits2::eNone)
                    .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
                    .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
                    .setOldLayout(vk::ImageLayout::eUndefined)
                    .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
                    .setSubresourceRange(toColorSubresourceRange);
        vk::DependencyInfo dependencyDescriptor{};
        dependencyDescriptor.setImageMemoryBarriers(toColor);
        encoder->GetCommandBuffer().pipelineBarrier2(dependencyDescriptor);

        return encoder;
    }

    void Device::EndEncoding(std::unique_ptr<Encoder> encoder) {
        auto swapchain = GetSwapchainInstance(encoder->GetTargetSwapchain());
        auto commandBuffer = encoder->GetCommandBuffer();
        auto backBuffer = swapchain->GetCurrentBackBuffer();

        vk::ImageSubresourceRange toColorSubresourceRange{};
        toColorSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1)
                    .setBaseMipLevel(0)
                    .setLevelCount(1);
        vk::ImageMemoryBarrier2 toPresent{};
        toPresent.setImage(GetTextureInstance(backBuffer)->Get())
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

    std::optional<GPUQueue> Device::GetQueue(Graphics::GPUQueueType queueType) {
        if (!_queues.contains(queueType)) return std::nullopt;
        return _queues[queueType];
    } 

    void Device::CreateInstance() {
        vk::ApplicationInfo appDescriptor{};
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
            #ifdef DEBUG
                "VK_EXT_debug_utils"
            #endif
        };

        uint32_t sdlCount;
        auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlCount);
        extensions.insert(extensions.end(), sdlExtensions, sdlExtensions + sdlCount);

        std::vector<const char*> layers = {
            #ifdef DEBUG
                "VK_LAYER_KHRONOS_validation",
            #endif
        };

        vk::InstanceCreateInfo instanceDescriptor{};
        instanceDescriptor.setPApplicationInfo(&appDescriptor)
                        .setPEnabledExtensionNames(extensions)
                        .setPEnabledLayerNames(layers)
                        .setFlags(instanceFlags);
        _instance = vk::createInstanceUnique(instanceDescriptor);
    }

    void Device::GetPhysicalDevice(Graphics::DeviceDesc desc) {
        auto physicalDevices = _instance->enumeratePhysicalDevices();
        
        std::map<int32_t, vk::PhysicalDevice> physicalDevicesRanked;
        for (auto& physicalDevice : physicalDevices) {
            auto properties = physicalDevice.getProperties();

            int32_t score = 0;

            switch (properties.deviceType) {
                case vk::PhysicalDeviceType::eDiscreteGpu:
                    score += desc.powerPreference == Graphics::GPUPowerPreference::HighPerformance ? 1000 : 100;
                    break;
                case vk::PhysicalDeviceType::eIntegratedGpu:
                    score += desc.powerPreference == Graphics::GPUPowerPreference::HighPerformance ? 500 : 500;
                    break;
                case vk::PhysicalDeviceType::eCpu:
                    score += desc.powerPreference == Graphics::GPUPowerPreference::HighPerformance ? 100 : 250;
                    break;
                case vk::PhysicalDeviceType::eOther:
                case vk::PhysicalDeviceType::eVirtualGpu:
                  break;
            }

            float capabilityWeight = (desc.powerPreference == Graphics::GPUPowerPreference::HighPerformance) ? 1.0f : 0.2f;
            score += static_cast<int32_t>(std::log2(properties.limits.maxImageDimension2D) * 5 * capabilityWeight);

            // TODO: Add feature checking

            physicalDevicesRanked[score] = physicalDevice;
        }
        _gpu = physicalDevicesRanked.rbegin()->second;
        std::cout << "Found GPU: " << _gpu.getProperties().deviceName << std::endl;
    }

    GPUQueue Device::GetSupportedQueue(std::vector<vk::QueueFamilyProperties> queueFamilies, Graphics::GPUQueueType type) {
        vk::QueueFlags requestedFlags;
        switch (type) {
            case Graphics::GPUQueueType::Unknown:
                break;
            case Graphics::GPUQueueType::Graphics:
                requestedFlags |= vk::QueueFlagBits::eGraphics;
                break;
            case Graphics::GPUQueueType::Transfer:
                requestedFlags |= vk::QueueFlagBits::eTransfer;
                break;
            case Graphics::GPUQueueType::Compute:
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
            .type = Graphics::GPUQueueType::Unknown,
            .family = 0,
            .queue = nullptr
        };
    }

    void Device::DiscoverQueues(Graphics::DeviceDesc desc) {
        auto queueFamilies = _gpu.getQueueFamilyProperties();

        for (const auto& queueType : desc.desiredQueues) {
            if (queueType == Graphics::GPUQueueType::Unknown) continue;
            if (_queues.contains(queueType)) continue;
            
            vk::QueueFlags requestedFlags;
            switch (queueType) {
                case Graphics::GPUQueueType::Unknown:
                    break;
                case Graphics::GPUQueueType::Graphics:
                    requestedFlags |= vk::QueueFlagBits::eGraphics;
                    break;
                case Graphics::GPUQueueType::Transfer:
                    requestedFlags |= vk::QueueFlagBits::eTransfer;
                    break;
                case Graphics::GPUQueueType::Compute:
                    requestedFlags |= vk::QueueFlagBits::eCompute;
                    break;
            }

            auto queue = GetSupportedQueue(queueFamilies, queueType);
            if (queue.type == Graphics::GPUQueueType::Unknown) continue;
            _queues[queueType] = queue;
        }

        uint32_t totalSatisfied = 0;
        std::unordered_map<Graphics::GPUQueueType, uint32_t> satisfiedQueues;
        for (const auto& queue : _queues) {
            bool found = false;
            for (const auto& queueRequested : desc.desiredQueues) {
                if (queue.first == queueRequested) {
                    satisfiedQueues[queue.first] += 1;
                    totalSatisfied += 1;

                    if (satisfiedQueues[queue.first] > 1) {
                        std::cout << "Reduntant " << Graphics::GPUQueueTypeToString(queue.first) << " queue was specified, please remove this duplicate!" << std::endl;
                    } else {
                        std::cout << "Found " << Graphics::GPUQueueTypeToString(queue.first) << " queue!" << std::endl;
                    }
                    found = true;
                }
            }

            if (!found) {
                std::cerr << "Failed to find " << Graphics::GPUQueueTypeToString(queue.first) << " queue!" << std::endl;
            }
        }

        std::cout << "Satisfied " << totalSatisfied << " of " << desc.desiredQueues.size() << " queues!" << std::endl;
    }

    void Device::CreateDevice() {
         auto queueFamilies = _gpu.getQueueFamilyProperties();

        // Vulkan requires unique queue families, it's possible
        // we may have same family indices so we must accumulate queue count to avoid this
        std::map<uint32_t, uint32_t> uniqueFamilies;
        for (const auto& queue : _queues | std::ranges::views::values) {
            uniqueFamilies[queue.family]++;
        }

        std::unordered_map<uint32_t, std::vector<float>> queuePriorities;

        std::vector<vk::DeviceQueueCreateInfo> deviceQueueDescriptors;
        for (const auto& [family, count] : uniqueFamilies) {
            auto availableQueueCount = queueFamilies[family].queueCount;
            auto actualQueueCount = std::min(count, availableQueueCount);

            queuePriorities[family] = std::vector<float>(actualQueueCount, 1.0f);
            vk::DeviceQueueCreateInfo deviceQueueDescriptor;
            deviceQueueDescriptor.setQueuePriorities(queuePriorities[family])
                                .setQueueCount(actualQueueCount)
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

        vk::PhysicalDeviceVulkan13Features vulkan13Features{};
        vulkan13Features.setDynamicRendering(true)
                        .setSynchronization2(true);

        vk::DeviceCreateInfo deviceDescriptor{};
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
            PANIC("Failed to create device");
        }
    }

    void Device::CreateCommandPool() {
        vk::CommandPoolCreateInfo commandPoolDescriptor{};
        commandPoolDescriptor.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                        .setQueueFamilyIndex(_queues[Graphics::GPUQueueType::Graphics].family);
        _commandPool = _device->createCommandPoolUnique(commandPoolDescriptor);
    }

    void Device::CreateCommandBuffers() {
        vk::CommandBufferAllocateInfo commandBufferDescriptor{};
            commandBufferDescriptor.setCommandBufferCount(2)
                        .setCommandPool(_commandPool.get())
                        .setLevel(vk::CommandBufferLevel::ePrimary);
        _commandBuffers = _device->allocateCommandBuffersUnique(commandBufferDescriptor);
    }

    void Device::CreateDescriptorPool() {
        vk::DescriptorPoolSize poolSize{};
        poolSize.setType(vk::DescriptorType::eUniformBuffer)
                .setDescriptorCount(1000);
        
        vk::DescriptorPoolCreateInfo descriptorPoolDescriptor{};
        descriptorPoolDescriptor.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                    .setMaxSets(1000)
                    .setPoolSizes(poolSize);
        _descriptorPool = _device->createDescriptorPoolUnique(descriptorPoolDescriptor);
    }
}