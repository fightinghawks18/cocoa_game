#pragma once

#include <vulkan/vulkan.hpp>
#include "swapchain.h"

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
        void SetViewport(vk::Viewport viewport);
        void SetScissor(vk::Rect2D scissor);

        [[nodiscard]] Swapchain* GetTargetSwapchain() { return _swapchain; }
        [[nodiscard]] vk::CommandBuffer GetCommandBuffer() { return _cmd; }
    private:
        vk::CommandBuffer _cmd;
        Swapchain* _swapchain;
    };
}