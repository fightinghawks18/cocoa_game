#pragma once

#include <vulkan/vulkan.hpp>

#include "common.h"
#include "handles.h"

namespace Cocoa::Vulkan {
    enum class GPUPassLoadOp {
        Clear,
        Load,
        DontCare
    };

    enum class GPUPassStoreOp {
        Store,
        DontCare
    };

    inline vk::AttachmentLoadOp LoadOpToVk(GPUPassLoadOp op) {
        switch (op) {
            case GPUPassLoadOp::Clear:
                return vk::AttachmentLoadOp::eClear;
            case GPUPassLoadOp::Load:
                return vk::AttachmentLoadOp::eLoad;
            case GPUPassLoadOp::DontCare:
                return vk::AttachmentLoadOp::eDontCare;
        }
    }

    inline vk::AttachmentStoreOp StoreOpToVk(GPUPassStoreOp op) {
        switch (op) {
            case GPUPassStoreOp::Store:
                return vk::AttachmentStoreOp::eStore;
            case GPUPassStoreOp::DontCare:
                return vk::AttachmentStoreOp::eDontCare;
        }
    }

    struct GPUColorPassDesc {
        TextureHandle texture;
        std::array<float, 4> clearColor;
        GPUPassLoadOp loadOp;
        GPUPassStoreOp storeOp;
    };

    struct GPUDepthPassDesc {
        TextureHandle texture;
        float depth;
        uint32_t stencil;
        GPUPassLoadOp loadOp;
        GPUPassStoreOp storeOp;
    };

    struct GPUPassDesc {
        std::vector<GPUColorPassDesc> colorPasses;
        const GPUDepthPassDesc* depthPass = nullptr;
        Rect renderArea;
        uint32_t viewMask;
        uint32_t layerCount;
    };

    struct EncoderDesc {
        SwapchainHandle swapchain;
        vk::CommandBuffer cmd;  
    };

    class Device;
    class Encoder {
    public:
        Encoder(Device* device, EncoderDesc desc);
        ~Encoder();

        void StartRendering(GPUPassDesc passDesc);
        void EndRendering();
        void SetRenderPipeline(RenderPipelineHandle renderPipeline);
        void SetBindGroup(PipelineLayoutHandle pipelineLayout, BindGroupHandle bindGroup);
        void SetVertexBuffer(BufferHandle vertexBuffer);
        void SetIndexBuffer(BufferHandle indexBuffer);
        void SetViewport(Viewport viewport);
        void SetScissor(Rect scissor);
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