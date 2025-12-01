#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>

#include "../graphics/handles.h"
#include "vma.h"
#include "common.h"
#include "encoder.h"

#include "../graphics/descriptors.h"

#include "swapchain.h"
#include "resources/buffer.h"
#include "resources/texture.h"
#include "resources/sampler.h"
#include "resources/bind_group.h"
#include "resources/pipeline_layout.h"
#include "resources/render_pipeline.h"
#include "resources/shader_module.h"

namespace Cocoa::Vulkan {
    class Device {
    public:
        Device(Graphics::DeviceDesc desc);
        ~Device();

        Graphics::SurfaceHandle CreateSurface(SDL_Window* window);
        Graphics::SwapchainHandle CreateSwapchain(Graphics::SwapchainDesc swapchainDesc);
        Graphics::BufferHandle CreateBuffer(Graphics::BufferDesc bufferDesc);
        Graphics::PipelineLayoutHandle CreatePipelineLayout(Graphics::PipelineLayoutDesc pipelineLayoutDesc);
        Graphics::RenderPipelineHandle CreateRenderPipeline(Graphics::PipelineDesc renderPipelineDesc);
        Graphics::SamplerHandle CreateSampler(vk::SamplerCreateInfo samplerDesc);
        Graphics::ShaderModuleHandle CreateShaderModule(Graphics::ShaderModuleDesc shaderModuleDesc);
        Graphics::TextureHandle CreateTexture(Graphics::TextureDesc textureDesc);
        Graphics::BindGroupHandle CreateBindGroup(Graphics::BindGroupDesc bindGroupDesc);

        void DestroySurface(Graphics::SurfaceHandle surface);
        void DestroySwapchain(Graphics::SwapchainHandle swapchain);
        void DestroyBuffer(Graphics::BufferHandle buffer);
        void DestroyPipelineLayout(Graphics::PipelineLayoutHandle pipelineLayout);
        void DestroyRenderPipeline(Graphics::RenderPipelineHandle renderPipeline);
        void DestroySampler(Graphics::SamplerHandle sampler);
        void DestroyShaderModule(Graphics::ShaderModuleHandle shaderModule);
        void DestroyTexture(Graphics::TextureHandle texture);
        void DestroyBindGroup(Graphics::BindGroupHandle bindGroup);

        [[nodiscard]] Surface* GetSurfaceInstance(Graphics::SurfaceHandle surface);
        [[nodiscard]] Swapchain* GetSwapchainInstance(Graphics::SwapchainHandle swapchain);
        [[nodiscard]] Buffer* GetBufferInstance(Graphics::BufferHandle buffer);
        [[nodiscard]] PipelineLayout* GetPipelineLayoutInstance(Graphics::PipelineLayoutHandle pipelineLayout);
        [[nodiscard]] RenderPipeline* GetRenderPipelineInstance(Graphics::RenderPipelineHandle renderPipeline);
        [[nodiscard]] Sampler* GetSamplerInstance(Graphics::SamplerHandle sampler);
        [[nodiscard]] ShaderModule* GetShaderModuleInstance(Graphics::ShaderModuleHandle shaderModule);
        [[nodiscard]] Texture* GetTextureInstance(Graphics::TextureHandle texture);
        [[nodiscard]] BindGroup* GetBindGroupInstance(Graphics::BindGroupHandle bindGroup);

        std::unique_ptr<Encoder> Encode(Graphics::SwapchainHandle swapchain);
        void EndEncoding(std::unique_ptr<Encoder> encoder);

        [[nodiscard]] std::optional<GPUQueue> GetQueue(Graphics::GPUQueueType queueType);
        [[nodiscard]] vk::Instance GetInstance() { return _instance.get(); }
        [[nodiscard]] vk::PhysicalDevice GetGPU() { return _gpu; }
        [[nodiscard]] vk::Device GetDevice() { return _device.get(); }
        [[nodiscard]] VmaAllocator GetAllocator() { return _allocator; }
        [[nodiscard]] vk::DescriptorPool GetDescriptorPool() { return _descriptorPool.get(); }
    private:
        vk::UniqueInstance _instance;
        vk::PhysicalDevice _gpu;
        vk::UniqueDevice _device;
        std::unordered_map<Graphics::GPUQueueType, GPUQueue> _queues;
        VmaAllocator _allocator;

        vk::UniqueCommandPool _commandPool;
        std::vector<vk::UniqueCommandBuffer> _commandBuffers;
        uint32_t _frame = 0;

        vk::UniqueDescriptorPool _descriptorPool;

        std::optional<Tools::ResourceManager<Surface>> _surfaceResources;
        std::optional<Tools::ResourceManager<Swapchain>> _swapchainResources;
        std::optional<Tools::ResourceManager<Buffer>> _bufferResources;
        std::optional<Tools::ResourceManager<PipelineLayout>> _pipelineLayoutResources;
        std::optional<Tools::ResourceManager<RenderPipeline>> _renderPipelineResources;
        std::optional<Tools::ResourceManager<Sampler>> _samplerResources;
        std::optional<Tools::ResourceManager<ShaderModule>> _shaderModuleResources;
        std::optional<Tools::ResourceManager<Texture>> _textureResources;
        std::optional<Tools::ResourceManager<BindGroup>> _bindGroupResources;

        void CreateInstance();
        void GetPhysicalDevice(Graphics::DeviceDesc desc);
        GPUQueue GetSupportedQueue(std::vector<vk::QueueFamilyProperties> queueFamilies, Graphics::GPUQueueType type);
        void DiscoverQueues(Graphics::DeviceDesc desc);
        void CreateDevice();
        void CreateAllocator();
        void CreateCommandPool();
        void CreateCommandBuffers();
        void CreateDescriptorPool();
    };
}