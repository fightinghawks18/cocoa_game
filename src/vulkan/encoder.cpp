#include "encoder.h"
#include "device.h"

namespace Cocoa::Vulkan {
    Encoder::Encoder(Device* device, EncoderDesc desc) : _device(device), _cmd(desc.cmd), _swapchain(desc.swapchain) {
        _cmd.reset();
        vk::CommandBufferBeginInfo commandBeginDescriptor{};
        commandBeginDescriptor.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        _cmd.begin(commandBeginDescriptor);
    }

    Encoder::~Encoder() {
        _cmd.end();
    }

    void Encoder::StartRendering(GPUPassDesc passDesc) {
        std::vector<vk::RenderingAttachmentInfo> renderColorDescs;
        renderColorDescs.reserve(passDesc.colorPasses.size());
        for (const auto& colorPass : passDesc.colorPasses) {
            vk::ClearColorValue clearColorValue{};
            clearColorValue.setFloat32(colorPass.clearColor);

            vk::ClearValue colorClear;
            colorClear.setColor(clearColorValue);

            vk::RenderingAttachmentInfo colorAttachment{};
            colorAttachment.setClearValue(colorClear)
                        .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
                        .setLoadOp(LoadOpToVk(colorPass.loadOp))
                        .setStoreOp(StoreOpToVk(colorPass.storeOp))
                        .setImageView(_device->GetTextureInstance(colorPass.texture)->GetView());
            renderColorDescs.push_back(colorAttachment);
        }

        vk::RenderingAttachmentInfo* renderDepthDesc = nullptr;
        if (passDesc.depthPass) {
            vk::RenderingAttachmentInfo depthAttachment{};

            vk::ClearValue depthClear;
            depthClear.setDepthStencil({passDesc.depthPass->depth, passDesc.depthPass->stencil});

            depthAttachment.setClearValue(depthClear)
                        .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                        .setLoadOp(LoadOpToVk(passDesc.depthPass->loadOp))
                        .setStoreOp(StoreOpToVk(passDesc.depthPass->storeOp))
                        .setImageView(_device->GetTextureInstance(passDesc.depthPass->texture)->GetView());
            renderDepthDesc = &depthAttachment;
        }

        vk::RenderingInfo renderDesc{};
        renderDesc.setColorAttachments(renderColorDescs)
                .setPDepthAttachment(renderDepthDesc)
                .setRenderArea(vk::Rect2D(
                    {passDesc.renderArea.offset.x, passDesc.renderArea.offset.y},
                    {passDesc.renderArea.extent.w, passDesc.renderArea.extent.h}
                ))
                .setViewMask(passDesc.viewMask)
                .setLayerCount(passDesc.layerCount);

        _cmd.beginRendering(renderDesc);
    }

    void Encoder::EndRendering() {
        _cmd.endRendering();
    }

    void Encoder::SetRenderPipeline(RenderPipelineHandle renderPipeline) {
        _cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _device->GetRenderPipelineInstance(renderPipeline)->Get());
    }

    void Encoder::SetBindGroup(PipelineLayoutHandle pipelineLayout, BindGroupHandle bindGroup) {
        auto set = _device->GetBindGroupInstance(bindGroup)->GetBinding();
        _cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _device->GetPipelineLayoutInstance(pipelineLayout)->Get(), 0, set, nullptr);
    }
    
    void Encoder::SetVertexBuffer(BufferHandle vertexBuffer) {
        vk::Buffer buffers[] = {_device->GetBufferInstance(vertexBuffer)->Get()};
        vk::DeviceSize offsets[] = {0};
        _cmd.bindVertexBuffers(0, 1, buffers, offsets);
    }

    void Encoder::SetIndexBuffer(BufferHandle indexBuffer) {
        _cmd.bindIndexBuffer(_device->GetBufferInstance(indexBuffer)->Get(), 0, vk::IndexType::eUint16);
    }

    void Encoder::SetViewport(Viewport viewport) {
        _cmd.setViewport(0, vk::Viewport(
            viewport.offset.x,
            viewport.offset.y,
            viewport.extent.w,
            viewport.extent.h,
            viewport.minDepth,
            viewport.maxDepth
        ));
    }

    void Encoder::SetScissor(Rect scissor) {
        _cmd.setScissor(0, vk::Rect2D(
            {scissor.offset.x, scissor.offset.y},
            {scissor.extent.w, scissor.extent.h}
        ));
    }

    void Encoder::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t vertexOffset, uint32_t firstInstance) {
        _cmd.drawIndexed(indexCount, instanceCount, firstVertex, vertexOffset, firstInstance);
    }

    void Encoder::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
        _cmd.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }
}