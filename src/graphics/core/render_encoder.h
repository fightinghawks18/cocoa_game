#pragma once

#include "../utils/descriptors.h"
#include "../utils/handles.h"
#include "../utils/types.h"

namespace Cocoa::Graphics {
    class RenderDevice;

    struct RenderEncoderState
    {
        GFXRenderPipelineHandle* currentPipeline = nullptr;
    };

    class RenderEncoder
    {
      public:
        virtual ~RenderEncoder() = default;

        virtual void TransitionTextureState(GPUTextureHandle& texture, GPUTextureState newState) = 0;

        virtual void UploadBufferDataToTexture(GPUBufferHandle& srcBuffer, GPUTextureHandle& dstTexture) = 0;

        virtual void StartRenderPass(const GPUPassDesc& renderPassDescriptor) = 0;

        virtual void SetRenderPipeline(GFXRenderPipelineHandle& renderPipeline) = 0;

        virtual void SetOutputTransform(const OutputTransform& outputTransform) = 0;

        virtual void SetRenderArea(const RenderArea& renderArea) = 0;

        virtual void SetVertexBuffer(GPUBufferHandle& vertexBuffer) = 0;

        virtual void SetIndexBuffer(GPUBufferHandle& indexBuffer) = 0;

        virtual void SetBindGroup(GPUBindGroupHandle& bindGroup) = 0;

        virtual void EndRenderPass() = 0;

        virtual void Stop() = 0;

        template <typename T> [[nodiscard]] T* As() { return static_cast<T*>(this); }
    protected:
        RenderEncoderState _state = {};
    };
}
