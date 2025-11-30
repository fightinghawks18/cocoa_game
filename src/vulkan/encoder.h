#pragma once

#include <vulkan/vulkan.hpp>

#include "handles.h"

namespace Cocoa::Vulkan {
    struct EncoderDesc {
        SwapchainHandle swapchain;
        vk::CommandBuffer cmd;  
    };

    class Device;
    class Encoder {
    public:
        Encoder(Device* device, EncoderDesc desc);
        ~Encoder();

        void StartRendering(vk::RenderingInfo renderDesc);
        void EndRendering();
        void SetRenderPipeline(RenderPipelineHandle renderPipeline);
        void SetBindGroup(PipelineLayoutHandle pipelineLayout, BindGroupHandle bindGroup);
        void SetVertexBuffer(BufferHandle vertexBuffer);
        void SetIndexBuffer(BufferHandle indexBuffer);
        void SetViewport(vk::Viewport viewport);
        void SetScissor(vk::Rect2D scissor);
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t vertexOffset, uint32_t firstInstance);
        void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

        [[nodiscard]] SwapchainHandle GetTargetSwapchain() { return _swapchain; }
        [[nodiscard]] vk::CommandBuffer GetCommandBuffer() { return _cmd; }
    private:
        Device* _device;

        vk::CommandBuffer _cmd;
        SwapchainHandle _swapchain;
    };
}