#include "encoder.h"
#include "device.h"

#include "internal/helpers/enums.h"
#include "internal/helpers/types.h"

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

    void Encoder::TransitionTexture(Graphics::TextureHandle texture, Graphics::GPUTextureLayout newLayout) {
        auto textureInstance = _device->GetTextureInstance(texture);
        auto oldLayoutInfo = GetLayoutInfo(GPUTextureLayoutToVk(textureInstance->GetLayout()));
        auto newLayoutInfo = GetLayoutInfo(GPUTextureLayoutToVk(newLayout));

        vk::ImageSubresourceRange toColorSubresourceRange{};
        toColorSubresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setLayerCount(textureInstance->GetLayers())
                    .setBaseMipLevel(0)
                    .setLevelCount(textureInstance->GetLevels());
        vk::ImageMemoryBarrier2 transition{};
        transition.setImage(textureInstance->Get())
                    .setSrcStageMask(oldLayoutInfo.stage)
                    .setSrcAccessMask(oldLayoutInfo.access)
                    .setDstStageMask(newLayoutInfo.stage)
                    .setDstAccessMask(newLayoutInfo.access)
                    .setOldLayout(GPUTextureLayoutToVk(textureInstance->GetLayout()))
                    .setNewLayout(GPUTextureLayoutToVk(newLayout))
                    .setSubresourceRange(toColorSubresourceRange);
        vk::DependencyInfo dependencyDescriptor{};
        dependencyDescriptor.setImageMemoryBarriers(transition);
        _cmd.pipelineBarrier2(dependencyDescriptor);
        textureInstance->SetLayout(newLayout);
    }

    void Encoder::StartRendering(Graphics::GPUPassDesc passDesc) {
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
                        .setLoadOp(GPUPassLoadOpToVk(colorPass.loadOp))
                        .setStoreOp(GPUPassStoreOpToVk(colorPass.storeOp))
                        .setImageView(_device->GetTextureInstance(colorPass.texture)->GetView(0)->Get());
            renderColorDescs.push_back(colorAttachment);
        }

        vk::RenderingAttachmentInfo* renderDepthDesc = nullptr;
        if (passDesc.depthPass) {
            vk::RenderingAttachmentInfo depthAttachment{};

            vk::ClearValue depthClear;
            depthClear.setDepthStencil({passDesc.depthPass->depth, passDesc.depthPass->stencil});

            depthAttachment.setClearValue(depthClear)
                        .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
                        .setLoadOp(GPUPassLoadOpToVk(passDesc.depthPass->loadOp))
                        .setStoreOp(GPUPassStoreOpToVk(passDesc.depthPass->storeOp))
                        .setImageView(_device->GetTextureInstance(passDesc.depthPass->texture)->GetView(0)->Get());
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

    void Encoder::SetRenderPipeline(Graphics::RenderPipelineHandle renderPipeline) {
        _cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _device->GetRenderPipelineInstance(renderPipeline)->Get());
    }

    void Encoder::SetBindGroup(Graphics::PipelineLayoutHandle pipelineLayout, Graphics::BindGroupHandle bindGroup) {
        auto set = _device->GetBindGroupInstance(bindGroup)->GetBinding();
        _cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _device->GetPipelineLayoutInstance(pipelineLayout)->Get(), 0, set, nullptr);
    }
    
    void Encoder::SetVertexBuffer(Graphics::BufferHandle vertexBuffer) {
        vk::Buffer buffers[] = {_device->GetBufferInstance(vertexBuffer)->Get()};
        vk::DeviceSize offsets[] = {0};
        _cmd.bindVertexBuffers(0, 1, buffers, offsets);
    }

    void Encoder::SetIndexBuffer(Graphics::BufferHandle indexBuffer) {
        _cmd.bindIndexBuffer(_device->GetBufferInstance(indexBuffer)->Get(), 0, vk::IndexType::eUint16);
    }

    void Encoder::SetViewport(Graphics::Viewport viewport) {
        _cmd.setViewport(0, vk::Viewport(
            viewport.offset.x,
            viewport.offset.y,
            viewport.extent.w,
            viewport.extent.h,
            viewport.minDepth,
            viewport.maxDepth
        ));
    }

    void Encoder::SetScissor(Graphics::Rect scissor) {
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