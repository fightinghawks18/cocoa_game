#pragma once

#include <vulkan/vulkan.hpp>

#include "../graphics/handles.h"
#include "../graphics/descriptors.h"

namespace Cocoa::Vulkan {
    struct EncoderDesc {
        Graphics::SwapchainHandle swapchain;
        vk::CommandBuffer cmd;  
    };

    class Device;
    class Encoder {
    public:
        Encoder(Device* device, EncoderDesc desc);
        ~Encoder();

        void TransitionTexture(Graphics::TextureHandle texture, Graphics::GPUTextureLayout newLayout);
        void UploadBufferToImage(Graphics::BufferHandle srcBuffer, Graphics::TextureHandle dstTexture);
        void StartRendering(Graphics::GPUPassDesc passDesc);
        void EndRendering();
        void SetRenderPipeline(Graphics::RenderPipelineHandle renderPipeline);
        void SetBindGroup(Graphics::PipelineLayoutHandle pipelineLayout, Graphics::BindGroupHandle bindGroup);
        void SetVertexBuffer(Graphics::BufferHandle vertexBuffer);
        void SetIndexBuffer(Graphics::BufferHandle indexBuffer);
        void SetViewport(Graphics::Viewport viewport);
        void SetScissor(Graphics::Rect scissor);
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t vertexOffset, uint32_t firstInstance);
        void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
        void End();

        [[nodiscard]] Graphics::SwapchainHandle GetTargetSwapchain() { return _swapchain; }
        [[nodiscard]] vk::CommandBuffer GetCommandBuffer() { return _cmd; }
    private:
        Device* _device;

        bool _active = false;
        vk::CommandBuffer _cmd;
        Graphics::SwapchainHandle _swapchain;
    };
}