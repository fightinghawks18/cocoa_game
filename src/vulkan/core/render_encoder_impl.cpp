//
// Created by fightinghawks18 on 12/7/2025.
//

#include "render_encoder_impl.h"

#include "render_device_impl.h"

namespace Cocoa::Vulkan {
    RenderEncoderImpl::RenderEncoderImpl(
        RenderDeviceImpl& device, const vk::CommandBuffer commandBuffer, const Graphics::RenderEncoderDesc& desc
    )
        : _device(device), _cmd(commandBuffer), _submitQueueType(desc.submitQueue), _active(true)
    {
    }

    void RenderEncoderImpl::TransitionTextureState(
        Graphics::GPUTextureHandle& texture, const Graphics::GPUTextureState newState
    )
    {
        const auto textureInstance = _device.GetTexture(texture);
        const auto oldLayout = GPUTextureLayoutToVk(textureInstance->layout);
        const auto newLayout = GPUTextureLayoutToVk(newState);
        const auto [oldStage, oldAccess] = GetLayoutInfo(oldLayout);
        const auto [newStage, newAccess] = GetLayoutInfo(newLayout);

        vk::ImageSubresourceRange transitionSubresourceRange{};
        transitionSubresourceRange.setAspectMask(InferAspectMasks(textureInstance->format))
            .setBaseArrayLayer(0)
            .setLayerCount(textureInstance->layers)
            .setBaseMipLevel(0)
            .setLevelCount(textureInstance->levels);
        vk::ImageMemoryBarrier2 transition{};
        transition.setImage(textureInstance->image)
            .setSrcStageMask(oldStage)
            .setSrcAccessMask(oldAccess)
            .setDstStageMask(newStage)
            .setDstAccessMask(newAccess)
            .setOldLayout(oldLayout)
            .setNewLayout(newLayout)
            .setSubresourceRange(transitionSubresourceRange);
        vk::DependencyInfo dependencyDescriptor{};
        dependencyDescriptor.setImageMemoryBarriers(transition);
        _cmd.pipelineBarrier2(dependencyDescriptor);
        textureInstance->layout = newState;
    }

    void RenderEncoderImpl::UploadBufferDataToTexture(
        Graphics::GPUBufferHandle& srcBuffer, Graphics::GPUTextureHandle& dstTexture
    )
    {
        const auto buffer = _device.GetBuffer(srcBuffer);
        const auto texture = _device.GetTexture(dstTexture);

        const auto oldTextureLayout = texture->layout;
        TransitionTextureState(dstTexture, Graphics::GPUTextureState::TransferDst);

        vk::ImageSubresourceLayers uploadSubresourceLayers{};
        uploadSubresourceLayers.setAspectMask(InferAspectMasks(texture->format))
            .setMipLevel(0)
            .setBaseArrayLayer(0)
            .setLayerCount(1);

        vk::BufferImageCopy region{};
        region.setBufferOffset(0)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageOffset(vk::Offset3D(0, 0, 0))
            .setImageExtent(vk::Extent3D(texture->extent.w, texture->extent.h, texture->extent.d))
            .setImageSubresource(uploadSubresourceLayers);

        _cmd.copyBufferToImage(buffer->buffer, texture->image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

        auto finalLayout = oldTextureLayout;
        if (finalLayout == Graphics::GPUTextureState::Unknown) {
            finalLayout = Graphics::GPUTextureState::ShaderReadOnly;
        }

        TransitionTextureState(dstTexture, finalLayout);
    }

    void RenderEncoderImpl::StartRenderPass(const Graphics::GPUPassDesc& renderPassDescriptor)
    {
        vk::RenderingInfo renderDescriptor{};

        std::vector<vk::RenderingAttachmentInfo> renderColorDescriptors;
        renderColorDescriptors.reserve(renderPassDescriptor.colorPasses.size());
        for (const auto& [view, clearColor, loadOp, storeOp] : renderPassDescriptor.colorPasses) {
            vk::ClearColorValue clearColorValue{};
            clearColorValue.setFloat32(clearColor);

            vk::ClearValue colorClear;
            colorClear.setColor(clearColorValue);

            vk::RenderingAttachmentInfo colorAttachment{};
            colorAttachment.setClearValue(colorClear)
                .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
                .setLoadOp(GPUPassLoadOpToVk(loadOp))
                .setStoreOp(GPUPassStoreOpToVk(storeOp))
                .setImageView(_device.GetTextureView(*view)->view.get());
            renderColorDescriptors.push_back(colorAttachment);
        }

        std::optional<vk::RenderingAttachmentInfo> renderDepthDescriptor;
        if (renderPassDescriptor.depthPass) {
            vk::RenderingAttachmentInfo depthAttachment{};

            vk::ClearValue depthClear;
            depthClear.setDepthStencil({renderPassDescriptor.depthPass->depth, renderPassDescriptor.depthPass->stencil}
            );

            depthAttachment.setClearValue(depthClear)
                .setImageLayout(
                    renderPassDescriptor.depthPass->useStencil ? vk::ImageLayout::eDepthStencilAttachmentOptimal
                                                               : vk::ImageLayout::eDepthAttachmentOptimal
                )
                .setLoadOp(GPUPassLoadOpToVk(renderPassDescriptor.depthPass->loadOp))
                .setStoreOp(GPUPassStoreOpToVk(renderPassDescriptor.depthPass->storeOp))
                .setResolveMode(vk::ResolveModeFlagBits::eNone)
                .setImageView(_device.GetTextureView(*renderPassDescriptor.depthPass->view)->view.get());
            renderDepthDescriptor = depthAttachment;
        }

        renderDescriptor.setPStencilAttachment(
            renderPassDescriptor.depthPass && renderPassDescriptor.depthPass->useStencil &&
                    renderDepthDescriptor.has_value()
                ? &renderDepthDescriptor.value()
                : nullptr
        );

        renderDescriptor.setColorAttachments(renderColorDescriptors)
            .setPDepthAttachment(renderDepthDescriptor.has_value() ? &renderDepthDescriptor.value() : nullptr)
            .setRenderArea(
                vk::Rect2D(
                    {renderPassDescriptor.renderArea.offset.x, renderPassDescriptor.renderArea.offset.y},
                    {renderPassDescriptor.renderArea.scale.w, renderPassDescriptor.renderArea.scale.h}
                )
            )
            .setViewMask(renderPassDescriptor.viewMask)
            .setLayerCount(renderPassDescriptor.layerCount);

        _cmd.beginRendering(renderDescriptor);
    }
    void RenderEncoderImpl::SetRenderPipeline(Graphics::GFXRenderPipelineHandle& renderPipeline)
    {
        _cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _device.GetPipeline(renderPipeline)->renderPipeline.get());
        _state.currentPipeline = &renderPipeline;
    }
    void RenderEncoderImpl::SetOutputTransform(const Graphics::OutputTransform& outputTransform)
    {
        _cmd.setViewport(
            0, vk::Viewport(
                   outputTransform.offset.x, outputTransform.offset.y, outputTransform.scale.w, outputTransform.scale.h,
                   outputTransform.minDepth, outputTransform.maxDepth
               )
        );
    }
    void RenderEncoderImpl::SetRenderArea(const Graphics::RenderArea& renderArea)
    {
        _cmd.setScissor(
            0, vk::Rect2D({renderArea.offset.x, renderArea.offset.y}, {renderArea.scale.w, renderArea.scale.h})
        );
    }
    void RenderEncoderImpl::SetVertexBuffer(Graphics::GPUBufferHandle& vertexBuffer)
    {
        const vk::Buffer buffers[] = {_device.GetBuffer(vertexBuffer)->buffer};
        constexpr vk::DeviceSize offsets[] = {0};
        _cmd.bindVertexBuffers(0, 1, buffers, offsets);
    }
    void RenderEncoderImpl::SetIndexBuffer(Graphics::GPUBufferHandle& indexBuffer)
    {
        _cmd.bindIndexBuffer(_device.GetBuffer(indexBuffer)->buffer, 0, vk::IndexType::eUint16);
    }
    void RenderEncoderImpl::SetBindGroup(Graphics::GPUBindGroupHandle& bindGroup)
    {
        const auto currentPipeline = _device.GetPipeline(*_state.currentPipeline);
        const auto set = _device.GetBindGroup(bindGroup);
        _cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _device.GetPipelineLayout(currentPipeline->pipelineLayout)->pipelineLayout.get(), 0, set->set.get(), nullptr
        );
    }
    void RenderEncoderImpl::EndRenderPass()
    {
        _cmd.endRendering();
        _state = {};
    }
    void RenderEncoderImpl::Stop()
    {
        _cmd.end();
        _cmd = nullptr;
        _active = false;
    }
}