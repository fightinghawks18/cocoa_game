#include "encoder.h"

namespace Cocoa::Vulkan {
    Encoder::Encoder(EncoderDesc desc) : _cmd(desc.cmd), _swapchain(desc.swapchain) {
        _cmd.reset();
        vk::CommandBufferBeginInfo commandBeginDescriptor{};
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

    void Encoder::SetRenderPipeline(RenderPipeline* renderPipeline) {
        _cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, renderPipeline->Get());
    }

    void Encoder::SetViewport(vk::Viewport viewport) {
        _cmd.setViewport(0, viewport);
    }

    void Encoder::SetScissor(vk::Rect2D scissor) {
        _cmd.setScissor(0, scissor);
    }

    void Encoder::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
        _cmd.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }
}