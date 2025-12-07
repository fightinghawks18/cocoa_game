//
// Created by fightinghawks18 on 12/7/2025.
//

#pragma once

#include "../../graphics/core/render_encoder.h"
#include "../utils/common.h"

namespace Cocoa::Vulkan {
    class RenderDeviceImpl;

    class RenderEncoderImpl final : public Graphics::RenderEncoder
    {
      public:
        RenderEncoderImpl(
            RenderDeviceImpl& device, vk::CommandBuffer commandBuffer, const Graphics::RenderEncoderDesc& desc
        );
        ~RenderEncoderImpl() override = default;
        void TransitionTextureState(Graphics::GPUTextureHandle& texture, Graphics::GPUTextureState newState) override;
        void UploadBufferDataToTexture(
            Graphics::GPUBufferHandle& srcBuffer, Graphics::GPUTextureHandle& dstTexture
        ) override;
        void StartRenderPass(const Graphics::GPUPassDesc& renderPassDescriptor) override;
        void SetRenderPipeline(Graphics::GFXRenderPipelineHandle& renderPipeline) override;
        void SetOutputTransform(const Graphics::OutputTransform& outputTransform) override;
        void SetRenderArea(const Graphics::RenderArea& renderArea) override;
        void SetVertexBuffer(Graphics::GPUBufferHandle& vertexBuffer) override;
        void SetIndexBuffer(Graphics::GPUBufferHandle& indexBuffer) override;
        void SetBindGroup(Graphics::GPUBindGroupHandle& bindGroup) override;
        void EndRenderPass() override;
        void Stop() override;

        [[nodiscard]] vk::CommandBuffer GetCommandBuffer() const { return _cmd; }
        [[nodiscard]] Graphics::GPUQueueType GetSubmitQueueType() const { return _submitQueueType; }
      private:
        RenderDeviceImpl& _device;
        vk::CommandBuffer _cmd;
        Graphics::GPUQueueType _submitQueueType;

        bool _active = false;
    };

} // namespace Cocoa::Vulkan