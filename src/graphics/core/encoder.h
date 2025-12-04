#pragma once

#include "../utils/descriptors.h"
#include "../utils/types.h"
#include "../utils/handles.h"

namespace Cocoa::Graphics {
    class GFXEncoder;
    class GFXDevice;
    struct GFXEncoderImpl {
        GFXEncoderImpl(GFXEncoderDesc desc, GFXDevice& device) : _device(device), _desc(desc) {}
        virtual ~GFXEncoderImpl() {}
        virtual void StartRenderPass(const GPUPassDesc& renderPassDescriptor) = 0;
        virtual void SetRenderPipeline(const GFXRenderPipelineHandle& renderPipeline) = 0;
        virtual void SetOutputTransform(const OutputTransform& outputTransform) = 0;
        virtual void SetRenderArea(const RenderArea& renderArea) = 0;
        virtual void SetVertexBuffer(const GFXBufferHandle& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const GFXBufferHandle& indexBuffer) = 0;
        virtual void SetBindGroup(const GFXBindGroupHandle& bindGroup) = 0;
        virtual void EndRenderPass() = 0;
        virtual void Stop() = 0;
    protected:
        GFXDevice& _device;
        GFXEncoderDesc _desc;
    };

    class GFXEncoder {
    public:
        GFXEncoder(GFXDevice& device);
        ~GFXEncoder();
        
        void StartRenderPass(const GPUPassDesc& renderPassDescriptor) {
            _impl->StartRenderPass(renderPassDescriptor);
        }

        void SetRenderPipeline(const GFXRenderPipelineHandle& renderPipeline) {
            _impl->SetRenderPipeline(renderPipeline);
        }

        void SetOutputTransform(const OutputTransform& outputTransform) {
            _impl->SetOutputTransform(outputTransform);
        }

        void SetRenderArea(const RenderArea& renderArea) {
            _impl->SetRenderArea(renderArea);
        }

        void SetVertexBuffer(const GFXBufferHandle& vertexBuffer) {
            _impl->SetVertexBuffer(vertexBuffer);
        }

        void SetIndexBuffer(const GFXBufferHandle& indexBuffer) {
            _impl->SetIndexBuffer(indexBuffer);
        }

        void SetBindGroup(const GFXBindGroupHandle& bindGroup) {
            _impl->SetBindGroup(bindGroup);
        }

        void EndRenderPass() {
            _impl->EndRenderPass();
        }

        void Stop() {
            _impl->Stop();
        }

        [[nodiscard]] GFXEncoderImpl* GetImpl() { return _impl.get(); }
    private:
        GFXDevice& _device;

        std::unique_ptr<GFXEncoderImpl> _impl;
    };
}