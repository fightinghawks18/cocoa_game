//
// Created by fightinghawks18 on 12/7/2025.
//

#pragma once

#include "../../graphics/core/render_device.h"
#include "../utils/common.h"

namespace Cocoa::Vulkan {
    struct GPUQueue {
        Graphics::GPUQueueType type;
        uint32_t family;
        vk::Queue queue;
    };

    class RenderDeviceImpl final : Graphics::RenderDevice
    {
      public:
        explicit RenderDeviceImpl(const Graphics::RenderDeviceDesc& desc);
        ~RenderDeviceImpl() override;

        Graphics::RenderWindowHandle ConnectWindow(const Graphics::RenderWindowDesc& desc) override;

        Graphics::GPUBufferHandle CreateBuffer(const Graphics::GPUBufferDesc& desc) override;

        Graphics::GPUTextureHandle CreateTexture(const Graphics::GPUTextureDesc& desc) override;

        Graphics::GPUTextureViewHandle CreateTextureView(const Graphics::GPUTextureViewDesc& desc) override;

        Graphics::GPUSamplerHandle CreateSampler(const Graphics::GPUSamplerDesc& desc) override;

        Graphics::GPUBindGroupHandle CreateBindGroup(const Graphics::GPUBindGroupDesc& desc) override;

        Graphics::GPUBindGroupLayoutHandle CreateBindGroupLayout(const Graphics::GPUBindGroupLayoutDesc& desc) override;

        Graphics::GFXRenderPipelineHandle CreateRenderPipeline(const Graphics::GFXPipelineDesc& desc) override;

        Graphics::GFXPipelineLayoutHandle CreatePipelineLayout(const Graphics::GFXPipelineLayoutDesc& desc) override;

        Graphics::GFXShaderModuleHandle CreateShaderModule(const Graphics::GFXShaderModuleDesc& desc) override;

        void DisconnectWindow(Graphics::RenderWindowHandle& handle) override;

        void DestroyBuffer(Graphics::GPUBufferHandle& handle) override;

        void DestroyTexture(Graphics::GPUTextureHandle& handle) override;

        void DestroyTextureView(Graphics::GPUTextureViewHandle& handle) override;

        void DestroySampler(Graphics::GPUSamplerHandle& handle) override;

        void DestroyBindGroup(Graphics::GPUBindGroupHandle& handle) override;

        void DestroyBindGroupLayout(Graphics::GPUBindGroupLayoutHandle& handle) override;

        void DestroyRenderPipeline(Graphics::GFXRenderPipelineHandle& handle) override;

        void DestroyPipelineLayout(Graphics::GFXPipelineLayoutHandle& handle) override;

        void DestroyShaderModule(Graphics::GFXShaderModuleHandle& handle) override;

        void WaitForIdle() override;

        std::unique_ptr<Graphics::RenderEncoder> Encode(const Graphics::RenderEncoderDesc& encoderDesc) override;

        void EndEncoding(std::unique_ptr<Graphics::RenderEncoder> encoder) override;

        void EncodeImmediateCommands(Graphics::EncodeImmediateFun encodeFun,
                                     const Graphics::RenderEncoderDesc& encoderDesc) override;

        [[nodiscard]] std::optional<GPUQueue> GetQueue(Graphics::GPUQueueType queueType);
        [[nodiscard]] vk::Instance GetInstance() { return _instance.get(); }
        [[nodiscard]] vk::PhysicalDevice GetGPU() const { return _gpu; }
        [[nodiscard]] vk::Device GetDevice() { return _device.get(); }
        [[nodiscard]] VmaAllocator GetAllocator() const { return _allocator; }
        [[nodiscard]] vk::DescriptorPool GetDescriptorPool() { return _descriptorPool.get(); }
      private:
        vk::UniqueInstance _instance;
        vk::PhysicalDevice _gpu;
        vk::UniqueDevice _device;
        std::unordered_map<Graphics::GPUQueueType, GPUQueue> _queues;
        VmaAllocator _allocator{};
        vk::UniqueCommandPool _commandPool;
        std::vector<vk::UniqueCommandBuffer> _commandBuffers;
        vk::UniqueFence _immediateFence;
        vk::UniqueCommandBuffer _immediateCommandBuffer;
        vk::UniqueDescriptorPool _descriptorPool;
        uint32_t _frame = 0;

        void CreateInstance();
        void GetPhysicalDevice(const Graphics::RenderDeviceDesc& desc);
        static GPUQueue GetSupportedQueue(const std::vector<vk::QueueFamilyProperties>& queueFamilies, Graphics::GPUQueueType type);
        void DiscoverQueues(const Graphics::RenderDeviceDesc& desc);
        void CreateDevice();
        void CreateAllocator();
        void CreateCommandPool();
        void CreateCommandBuffers();
        void CreateImmediateResources();
        void CreateDescriptorPool();
    };
} // namespace Cocoa::Vulkan
