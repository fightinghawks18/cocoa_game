#pragma once

#include <vulkan/vulkan.hpp>

#include "swapchain.h"
#include "resources/render_pipeline.h"
#include "resources/buffer.h"
#include "resources/bind_group.h"
#include "resources/pipeline_layout.h"

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
        void SetBindGroup(PipelineLayout* pipelineLayout, BindGroup* bindGroup);
        void SetVertexBuffer(Buffer* vertexBuffer);
        void SetIndexBuffer(Buffer* indexBuffer);
        void SetViewport(vk::Viewport viewport);
        void SetScissor(vk::Rect2D scissor);
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t vertexOffset, uint32_t firstInstance);
        void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

        [[nodiscard]] Swapchain* GetTargetSwapchain() { return _swapchain; }
        [[nodiscard]] vk::CommandBuffer GetCommandBuffer() { return _cmd; }
    private:
        vk::CommandBuffer _cmd;
        Swapchain* _swapchain;
    };
}