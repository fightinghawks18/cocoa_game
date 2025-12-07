#pragma once

#include "../utils/descriptors.h"
#include "../utils/types.h"
#include "../utils/handles.h"

namespace Cocoa::Graphics
{
    class RenderDevice;
    class RenderEncoder
    {
        public:
            virtual ~RenderEncoder() = default;

            virtual void TransitionImage( GPUTextureHandle& texture, GPUTextureLayout newState ) = 0;

            virtual void StartRenderPass( const GPUPassDesc& renderPassDescriptor ) = 0;

            virtual void SetRenderPipeline( const GFXRenderPipelineHandle& renderPipeline ) = 0;

            virtual void SetOutputTransform( const OutputTransform& outputTransform ) = 0;

            virtual void SetRenderArea( const RenderArea& renderArea ) = 0;

            virtual void SetVertexBuffer( const GPUBufferHandle& vertexBuffer ) = 0;

            virtual void SetIndexBuffer( const GPUBufferHandle& indexBuffer ) = 0;

            virtual void SetBindGroup( const GPUBindGroupHandle& bindGroup ) = 0;

            virtual void EndRenderPass() = 0;

            virtual void Stop() = 0;

            template<typename T>
            [[nodiscard]] T* As()
            {
                return static_cast<T*>( this );
            }
    };
}
