//
// Created by fightinghawks18 on 12/7/2025.
//

#include "render_device_impl.h"

#include <SDL3/SDL_vulkan.h>
#include <map>
#include <ranges>

namespace Cocoa::Vulkan {
    RenderDeviceImpl::RenderDeviceImpl(const Graphics::RenderDeviceDesc& desc)
    {
        AddManager<Buffer>();
        AddManager<Texture>();
        AddManager<TextureView>();
        AddManager<BindGroup>();
        AddManager<Pipeline>();
        AddManager<PipelineLayout>();

        CreateInstance();
        GetPhysicalDevice(desc);
        DiscoverQueues(desc);
        CreateDevice();
        CreateAllocator();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateImmediateResources();
        CreateDescriptorPool();
    }

    RenderDeviceImpl::~RenderDeviceImpl()
    {
        _device->waitIdle();

        _resourceManagers.clear();

        _descriptorPool.reset();
        _commandBuffers.clear();
        _immediateCommandBuffer.reset();
        _immediateFence.reset();
        _commandPool.reset();

        vmaDestroyAllocator(_allocator);
        _allocator = nullptr;

        _queues.clear();
        _device.reset();
        _instance.reset();
    }

    Graphics::RenderWindowHandle RenderDeviceImpl::ConnectWindow(const Graphics::RenderWindowDesc& desc) {}

    Graphics::GPUBufferHandle RenderDeviceImpl::CreateBuffer(const Graphics::GPUBufferDesc& desc) {}

    Graphics::GPUTextureHandle RenderDeviceImpl::CreateTexture(const Graphics::GPUTextureDesc& desc) {}

    Graphics::GPUTextureViewHandle RenderDeviceImpl::CreateTextureView(const Graphics::GPUTextureViewDesc& desc) {}

    Graphics::GPUSamplerHandle RenderDeviceImpl::CreateSampler(const Graphics::GPUSamplerDesc& desc) {}

    Graphics::GPUBindGroupHandle RenderDeviceImpl::CreateBindGroup(const Graphics::GPUBindGroupDesc& desc) {}

    Graphics::GPUBindGroupLayoutHandle
    RenderDeviceImpl::CreateBindGroupLayout(const Graphics::GPUBindGroupLayoutDesc& desc)
    {
    }

    Graphics::GFXRenderPipelineHandle RenderDeviceImpl::CreateRenderPipeline(const Graphics::GFXPipelineDesc& desc) {}

    Graphics::GFXPipelineLayoutHandle
    RenderDeviceImpl::CreatePipelineLayout(const Graphics::GFXPipelineLayoutDesc& desc)
    {
    }

    Graphics::GFXShaderModuleHandle RenderDeviceImpl::CreateShaderModule(const Graphics::GFXShaderModuleDesc& desc) {}

    void RenderDeviceImpl::DisconnectWindow(Graphics::RenderWindowHandle& handle) {}

    void RenderDeviceImpl::DestroyBuffer(Graphics::GPUBufferHandle& handle) { GetManager<Buffer>()->Destroy(handle); }

    void RenderDeviceImpl::DestroyTexture(Graphics::GPUTextureHandle& handle)
    {
        GetManager<Texture>()->Destroy(handle);
    }

    void RenderDeviceImpl::DestroyTextureView(Graphics::GPUTextureViewHandle& handle)
    {
        GetManager<TextureView>()->Destroy(handle);
    }

    void RenderDeviceImpl::DestroySampler(Graphics::GPUSamplerHandle& handle) {}

    void RenderDeviceImpl::DestroyBindGroup(Graphics::GPUBindGroupHandle& handle)
    {
        GetManager<BindGroup>()->Destroy(handle);
    }

    void RenderDeviceImpl::DestroyBindGroupLayout(Graphics::GPUBindGroupLayoutHandle& handle) {}

    void RenderDeviceImpl::DestroyRenderPipeline(Graphics::GFXRenderPipelineHandle& handle)
    {
        GetManager<Pipeline>()->Destroy(handle);
    }

    void RenderDeviceImpl::DestroyPipelineLayout(Graphics::GFXPipelineLayoutHandle& handle)
    {
        GetManager<PipelineLayout>()->Destroy(handle);
    }

    void RenderDeviceImpl::DestroyShaderModule(Graphics::GFXShaderModuleHandle& handle) {}

    Buffer* RenderDeviceImpl::GetBuffer(Graphics::GPUBufferHandle& handle) { return GetManager<Buffer>()->Get(handle); }

    Texture* RenderDeviceImpl::GetTexture(Graphics::GPUTextureHandle& handle)
    {
        return GetManager<Texture>()->Get(handle);
    }

    TextureView* RenderDeviceImpl::GetTextureView(Graphics::GPUTextureViewHandle& handle)
    {
        return GetManager<TextureView>()->Get(handle);
    }

    BindGroup* RenderDeviceImpl::GetBindGroup(Graphics::GPUBindGroupHandle& handle)
    {
        return GetManager<BindGroup>()->Get(handle);
    }

    Pipeline* RenderDeviceImpl::GetPipeline(Graphics::GFXRenderPipelineHandle& handle)
    {
        return GetManager<Pipeline>()->Get(handle);
    }

    PipelineLayout* RenderDeviceImpl::GetPipelineLayout(Graphics::GFXPipelineLayoutHandle& handle)
    {
        return GetManager<PipelineLayout>()->Get(handle);
    }

    void RenderDeviceImpl::WaitForIdle() { _device->waitIdle(); }

    std::unique_ptr<Graphics::RenderEncoder> RenderDeviceImpl::Encode(const Graphics::RenderEncoderDesc& encoderDesc)
    {
        std::unique_ptr<Graphics::RenderEncoder> encoder = nullptr;
        return encoder;
    }

    void RenderDeviceImpl::EndEncoding(const std::unique_ptr<Graphics::RenderEncoder> encoder)
    {
        encoder->Stop();
        _frame = (_frame + 1) % 2;
    }

    void RenderDeviceImpl::EncodeImmediateCommands(
        const Graphics::EncodeImmediateFun encodeFun, const Graphics::RenderEncoderDesc& encoderDesc
    )
    {
        auto encoder = Encode(encoderDesc);
        encodeFun(encoder.get());
        encoder->Stop();
        encoder.reset();

        vk::CommandBufferSubmitInfo commandSubmitDescriptor{};
        commandSubmitDescriptor.setCommandBuffer(_immediateCommandBuffer.get());

        vk::SubmitInfo2 submitDescriptor{};
        submitDescriptor.setCommandBufferInfos(commandSubmitDescriptor);
        GetQueue(encoderDesc.submitQueue)->queue.submit2(submitDescriptor, _immediateFence.get());

        const auto result = _device->waitForFences(_immediateFence.get(), VK_TRUE, UINT64_MAX);
        if (result != vk::Result::eSuccess) {
            PANIC("Failed to wait for GPU to finish submission of immediate commands");
        }
        _device->resetFences(_immediateFence.get());
    }

    std::optional<GPUQueue> RenderDeviceImpl::GetQueue(const Graphics::GPUQueueType queueType)
    {
        if (!_queues.contains(queueType))
            return std::nullopt;
        return _queues[queueType];
    }

    void RenderDeviceImpl::CreateInstance()
    {
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

        std::vector extensions = {
#ifdef DEBUG
            "VK_EXT_debug_utils"
#endif
        };

        uint32_t sdlCount;
        auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlCount);
        extensions.insert(extensions.end(), sdlExtensions, sdlExtensions + sdlCount);

        std::vector layers = {
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
    void RenderDeviceImpl::GetPhysicalDevice(const Graphics::RenderDeviceDesc& desc)
    {
        const auto physicalDevices = _instance->enumeratePhysicalDevices();

        std::map<int32_t, vk::PhysicalDevice> physicalDevicesRanked;
        for (auto& physicalDevice : physicalDevices) {
            const auto properties = physicalDevice.getProperties();

            int32_t score = 0;

            switch (properties.deviceType) {
            case vk::PhysicalDeviceType::eDiscreteGpu:
                score += desc.powerPreference == Graphics::GPUPowerPreference::HighPerformance ? 1000 : 100;
                break;
            case vk::PhysicalDeviceType::eIntegratedGpu: score += 500; break;
            case vk::PhysicalDeviceType::eCpu:
                score += desc.powerPreference == Graphics::GPUPowerPreference::HighPerformance ? 100 : 250;
                break;
            case vk::PhysicalDeviceType::eOther:
            case vk::PhysicalDeviceType::eVirtualGpu: break;
            }

            const float capabilityWeight =
                desc.powerPreference == Graphics::GPUPowerPreference::HighPerformance ? 1.0f : 0.2f;
            score += static_cast<int32_t>(std::log2(properties.limits.maxImageDimension2D) * 5 * capabilityWeight);

            // TODO: Add feature checking

            physicalDevicesRanked[score] = physicalDevice;
        }
        _gpu = physicalDevicesRanked.rbegin()->second;
        PUSH_INFO("Found GPU: %s", _gpu.getProperties().deviceName);
    }
    GPUQueue RenderDeviceImpl::GetSupportedQueue(
        const std::vector<vk::QueueFamilyProperties>& queueFamilies, const Graphics::GPUQueueType type
    )
    {
        vk::QueueFlags requestedFlags;
        switch (type) {
        case Graphics::GPUQueueType::Unknown:  break;
        case Graphics::GPUQueueType::Graphics: requestedFlags |= vk::QueueFlagBits::eGraphics; break;
        case Graphics::GPUQueueType::Transfer: requestedFlags |= vk::QueueFlagBits::eTransfer; break;
        case Graphics::GPUQueueType::Compute:  requestedFlags |= vk::QueueFlagBits::eCompute; break;
        }

        for (uint32_t i = 0; const auto& queueFamily : queueFamilies) {
            if (!(queueFamily.queueFlags & requestedFlags)) {
                i++;
                continue;
            }

            return {.type = type, .family = i, .queue = nullptr};
        }

        return {.type = Graphics::GPUQueueType::Unknown, .family = 0, .queue = nullptr};
    }
    void RenderDeviceImpl::DiscoverQueues(const Graphics::RenderDeviceDesc& desc)
    {
        const auto queueFamilies = _gpu.getQueueFamilyProperties();

        for (const auto& queueType : desc.desiredQueues) {
            if (queueType == Graphics::GPUQueueType::Unknown)
                continue;
            if (_queues.contains(queueType))
                continue;

            vk::QueueFlags requestedFlags;
            switch (queueType) {
            case Graphics::GPUQueueType::Graphics: requestedFlags |= vk::QueueFlagBits::eGraphics; break;
            case Graphics::GPUQueueType::Transfer: requestedFlags |= vk::QueueFlagBits::eTransfer; break;
            case Graphics::GPUQueueType::Compute:  requestedFlags |= vk::QueueFlagBits::eCompute; break;
            default:                               break;
            }

            const auto queue = GetSupportedQueue(queueFamilies, queueType);
            if (queue.type == Graphics::GPUQueueType::Unknown)
                continue;
            _queues[queueType] = queue;
        }

        uint32_t totalSatisfied = 0;
        std::unordered_map<Graphics::GPUQueueType, uint32_t> satisfiedQueues;
        for (const auto& queue : _queues | std::views::keys) {
            bool found = false;
            for (const auto& queueRequested : desc.desiredQueues) {
                if (queue == queueRequested) {
                    satisfiedQueues[queue] += 1;
                    totalSatisfied += 1;

                    if (satisfiedQueues[queue] > 1) {
                        PUSH_WARN(
                            "Redundant %s queue was specified, please remove this duplicate!",
                            Graphics::GPUQueueTypeAsString(queue).c_str()
                        );
                    } else {
                        PUSH_INFO("Found %s queue!", Graphics::GPUQueueTypeAsString(queue).c_str());
                    }
                    found = true;
                }
            }

            if (!found) {
                PUSH_ERR("Failed to find %s queue!", Graphics::GPUQueueTypeAsString(queue).c_str());
            }
        }

        PUSH_INFO("Satisfied %d of %d queues!", totalSatisfied, desc.desiredQueues.size());
    }
    void RenderDeviceImpl::CreateDevice()
    {
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

            queuePriorities[family] = std::vector(actualQueueCount, 1.0f);
            vk::DeviceQueueCreateInfo deviceQueueDescriptor;
            deviceQueueDescriptor.setQueuePriorities(queuePriorities[family])
                .setQueueCount(actualQueueCount)
                .setQueueFamilyIndex(family);
            deviceQueueDescriptors.push_back(deviceQueueDescriptor);
        }

        std::vector deviceExtensions = {
            "VK_KHR_swapchain",
#ifdef __APPLE__
            "VK_KHR_portability_subset",
#endif
            "VK_KHR_synchronization2"
        };

        vk::PhysicalDeviceFeatures vkFeatures{};
        vkFeatures.setFillModeNonSolid(true);

        vk::PhysicalDeviceFeatures2 vkFeatures2{};
        vkFeatures2.setFeatures(vkFeatures);

        vk::PhysicalDeviceVulkan13Features vulkan13Features{};
        vulkan13Features.setPNext(&vkFeatures2).setDynamicRendering(true).setSynchronization2(true);

        vk::DeviceCreateInfo deviceDescriptor{};
        deviceDescriptor.setPNext(&vulkan13Features)
            .setPEnabledExtensionNames(deviceExtensions)
            .setQueueCreateInfos(deviceQueueDescriptors);
        _device = _gpu.createDeviceUnique(deviceDescriptor);

        for (auto& queue : _queues | std::views::values) {
            queue.queue = _device->getQueue(queue.family, 0);
        }
    }
    void RenderDeviceImpl::CreateAllocator()
    {
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
    void RenderDeviceImpl::CreateCommandPool()
    {
        vk::CommandPoolCreateInfo commandPoolDescriptor{};
        commandPoolDescriptor.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
            .setQueueFamilyIndex(_queues[Graphics::GPUQueueType::Graphics].family);
        _commandPool = _device->createCommandPoolUnique(commandPoolDescriptor);
    }
    void RenderDeviceImpl::CreateCommandBuffers()
    {
        vk::CommandBufferAllocateInfo commandBufferDescriptor{};
        commandBufferDescriptor.setCommandBufferCount(2)
            .setCommandPool(_commandPool.get())
            .setLevel(vk::CommandBufferLevel::ePrimary);
        _commandBuffers = _device->allocateCommandBuffersUnique(commandBufferDescriptor);
    }
    void RenderDeviceImpl::CreateImmediateResources()
    {
        vk::CommandBufferAllocateInfo commandBufferDescriptor{};
        commandBufferDescriptor.setCommandBufferCount(1)
            .setCommandPool(_commandPool.get())
            .setLevel(vk::CommandBufferLevel::ePrimary);
        auto buffers = _device->allocateCommandBuffersUnique(commandBufferDescriptor);
        _immediateCommandBuffer = std::move(buffers[0]);

        constexpr vk::FenceCreateInfo fenceDescriptor{};
        _immediateFence = _device->createFenceUnique(fenceDescriptor);
    }
    void RenderDeviceImpl::CreateDescriptorPool()
    {
        vk::DescriptorPoolSize bufferPoolSize{};
        bufferPoolSize.setType(vk::DescriptorType::eUniformBuffer).setDescriptorCount(1000);

        vk::DescriptorPoolSize imagePoolSize{};
        imagePoolSize.setType(vk::DescriptorType::eSampledImage).setDescriptorCount(1000);

        vk::DescriptorPoolSize samplerPoolSize{};
        samplerPoolSize.setType(vk::DescriptorType::eSampler).setDescriptorCount(1000);

        std::vector poolSizes = {bufferPoolSize, imagePoolSize, samplerPoolSize};

        vk::DescriptorPoolCreateInfo descriptorPoolDescriptor{};
        descriptorPoolDescriptor.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(1000)
            .setPoolSizes(poolSizes);
        _descriptorPool = _device->createDescriptorPoolUnique(descriptorPoolDescriptor);
    }
} // namespace Cocoa::Vulkan
