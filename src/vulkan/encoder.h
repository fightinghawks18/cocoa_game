#pragma once

#include <vulkan/vulkan.hpp>

#include "swapchain.h"
#include "resources/render_pipeline.h"

namespace Cocoa::Vulkan {
    struct EncoderDesc {
        Swapchain* swapchain;
        vk::CommandBuffer cmd;  
    };

    class Encoder {
    public:
        Encoder(EncoderDesc desc);
        ~Encoder();

        void StartRendering(vk::RenderingInfo renderDesc);
        void EndRendering();
        void SetRenderPipeline(RenderPipeline* renderPipeline);
        void SetViewport(vk::Viewport viewport);
        void SetScissor(vk::Rect2D scissor);
        void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

        [[nodiscard]] Swapchain* GetTargetSwapchain() { return _swapchain; }
        [[nodiscard]] vk::CommandBuffer GetCommandBuffer() { return _cmd; }
    private:
        vk::CommandBuffer _cmd;
        Swapchain* _swapchain;
    };
}