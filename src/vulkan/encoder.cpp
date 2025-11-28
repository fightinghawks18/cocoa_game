#include "encoder.h"

namespace Cocoa::Vulkan {
    Encoder::Encoder(EncoderDesc desc) : _cmd(desc.cmd), _swapchain(desc.swapchain) {
        _cmd.reset();
        vk::CommandBufferBeginInfo commandBeginDescriptor;
        commandBeginDescriptor.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        _cmd.begin(commandBeginDescriptor);
    }

    Encoder::~Encoder() {
        _cmd.end();
    }

    void Encoder::StartRendering(vk::RenderingInfo renderDesc) {
        _cmd.beginRendering(renderDesc);
    }

    void Encoder::EndRendering() {
        _cmd.endRendering();
    }

    void Encoder::SetViewport(vk::Viewport viewport) {
        _cmd.setViewport(0, viewport);
    }

    void Encoder::SetScissor(vk::Rect2D scissor) {
        _cmd.setScissor(0, scissor);
    }
}