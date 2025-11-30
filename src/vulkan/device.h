#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>

#include "handles.h"
#include "vma.h"
#include "common.h"
#include "encoder.h"

#include "swapchain.h"
#include "resources/buffer.h"
#include "resources/texture.h"
#include "resources/sampler.h"
#include "resources/bind_group.h"
#include "resources/pipeline_layout.h"
#include "resources/render_pipeline.h"
#include "resources/shader_module.h"

#include "../tools/resource_manager.h"

namespace Cocoa::Vulkan {
    struct DeviceDesc {
        SDL_Window* window;
        std::vector<GPUQueueType> desiredQueues;
        GPUPowerPreference powerPreference;
    };

    class Device {
    public:
        Device(DeviceDesc desc);
        ~Device();

        SurfaceHandle CreateSurface(SDL_Window* window);
        SwapchainHandle CreateSwapchain(SwapchainDesc swapchainDesc);
        BufferHandle CreateBuffer(BufferDesc bufferDesc);
        PipelineLayoutHandle CreatePipelineLayout(vk::PipelineLayoutCreateInfo pipelineLayoutDesc);
        RenderPipelineHandle CreateRenderPipeline(vk::GraphicsPipelineCreateInfo renderPipelineDesc);
        SamplerHandle CreateSampler(vk::SamplerCreateInfo samplerDesc);
        ShaderModuleHandle CreateShaderModule(vk::ShaderModuleCreateInfo shaderModuleDesc);
        TextureHandle CreateTexture(vk::ImageCreateInfo textureDesc, vk::ImageViewCreateInfo* textureViewDesc);
        BindGroupHandle CreateBindGroup(BindGroupDesc bindGroupDesc);

        void DestroySurface(SurfaceHandle surface);
        void DestroySwapchain(SwapchainHandle swapchain);
        void DestroyBuffer(BufferHandle buffer);
        void DestroyPipelineLayout(PipelineLayoutHandle pipelineLayout);
        void DestroyRenderPipeline(RenderPipelineHandle renderPipeline);
        void DestroySampler(SamplerHandle sampler);
        void DestroyShaderModule(ShaderModuleHandle shaderModule);
        void DestroyTexture(TextureHandle texture);
        void DestroyBindGroup(BindGroupHandle bindGroup);

        [[nodiscard]] Surface* GetSurfaceInstance(SurfaceHandle surface);
        [[nodiscard]] Swapchain* GetSwapchainInstance(SwapchainHandle swapchain);
        [[nodiscard]] Buffer* GetBufferInstance(BufferHandle buffer);
        [[nodiscard]] PipelineLayout* GetPipelineLayoutInstance(PipelineLayoutHandle pipelineLayout);
        [[nodiscard]] RenderPipeline* GetRenderPipelineInstance(RenderPipelineHandle renderPipeline);
        [[nodiscard]] Sampler* GetSamplerInstance(SamplerHandle sampler);
        [[nodiscard]] ShaderModule* GetShaderModuleInstance(ShaderModuleHandle shaderModule);
        [[nodiscard]] Texture* GetTextureInstance(TextureHandle texture);
        [[nodiscard]] BindGroup* GetBindGroupInstance(BindGroupHandle bindGroup);

        std::unique_ptr<Encoder> Encode(SwapchainHandle swapchain);
        void EndEncoding(std::unique_ptr<Encoder> encoder);

        [[nodiscard]] std::optional<GPUQueue> GetQueue(GPUQueueType queueType);
        [[nodiscard]] vk::Instance GetInstance() { return _instance.get(); }
        [[nodiscard]] vk::PhysicalDevice GetGPU() { return _gpu; }
        [[nodiscard]] vk::Device GetDevice() { return _device.get(); }
        [[nodiscard]] VmaAllocator GetAllocator() { return _allocator; }
        [[nodiscard]] vk::DescriptorPool GetDescriptorPool() { return _descriptorPool.get(); }
    private:
        vk::UniqueInstance _instance;
        vk::PhysicalDevice _gpu;
        vk::UniqueDevice _device;
        std::unordered_map<GPUQueueType, GPUQueue> _queues;
        VmaAllocator _allocator;

        vk::UniqueCommandPool _commandPool;
        std::vector<vk::UniqueCommandBuffer> _commandBuffers;
        uint32_t _frame = 0;

        vk::UniqueDescriptorPool _descriptorPool;

        std::unique_ptr<Tools::ResourceManager<Surface>> _surfaceResources;
        std::unique_ptr<Tools::ResourceManager<Swapchain>> _swapchainResources;
        std::unique_ptr<Tools::ResourceManager<Buffer>> _bufferResources;
        std::unique_ptr<Tools::ResourceManager<PipelineLayout>> _pipelineLayoutResources;
        std::unique_ptr<Tools::ResourceManager<RenderPipeline>> _renderPipelineResources;
        std::unique_ptr<Tools::ResourceManager<Sampler>> _samplerResources;
        std::unique_ptr<Tools::ResourceManager<ShaderModule>> _shaderModuleResources;
        std::unique_ptr<Tools::ResourceManager<Texture>> _textureResources;
        std::unique_ptr<Tools::ResourceManager<BindGroup>> _bindGroupResources;

        void CreateInstance();
        void GetPhysicalDevice(DeviceDesc desc);
        GPUQueue GetSupportedQueue(std::vector<vk::QueueFamilyProperties> queueFamilies, GPUQueueType type);
        void DiscoverQueues(DeviceDesc desc);
        void CreateDevice();
        void CreateAllocator();
        void CreateCommandPool();
        void CreateCommandBuffers();
        void CreateDescriptorPool();
    };
}