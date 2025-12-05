#pragma once

#include "../utils/descriptors.h"
#include "../utils/types.h"
#include "../utils/handles.h"

namespace Cocoa::Graphics
{
    class RenderEncoder;
    class RenderDevice;

    struct RenderEncoderImpl
    {
        virtual ~RenderEncoderImpl() = default;

        virtual void StartRenderPass( const GPUPassDesc& renderPassDescriptor ) = 0;

        virtual void SetRenderPipeline( const GFXRenderPipelineHandle& renderPipeline ) = 0;

        virtual void SetOutputTransform( const OutputTransform& outputTransform ) = 0;

        virtual void SetRenderArea( const RenderArea& renderArea ) = 0;

        virtual void SetVertexBuffer( const GPUBufferHandle& vertexBuffer ) = 0;

        virtual void SetIndexBuffer( const GPUBufferHandle& indexBuffer ) = 0;

        virtual void SetBindGroup( const GPUBindGroupHandle& bindGroup ) = 0;

        virtual void EndRenderPass() = 0;

        virtual void Stop() = 0;
    };

    class RenderEncoder
    {
        public:
            RenderEncoder( std::unique_ptr<RenderEncoderImpl> impl ) : _impl( std::move( impl ) )
            {
            }

            ~RenderEncoder();

            void StartRenderPass( const GPUPassDesc& renderPassDescriptor )
            {
                _impl->StartRenderPass( renderPassDescriptor );
            }

            void SetRenderPipeline( const GFXRenderPipelineHandle& renderPipeline )
            {
                _impl->SetRenderPipeline( renderPipeline );
            }

            void SetOutputTransform( const OutputTransform& outputTransform )
            {
                _impl->SetOutputTransform( outputTransform );
            }

            void SetRenderArea( const RenderArea& renderArea )
            {
                _impl->SetRenderArea( renderArea );
            }

            void SetVertexBuffer( const GPUBufferHandle& vertexBuffer )
            {
                _impl->SetVertexBuffer( vertexBuffer );
            }

            void SetIndexBuffer( const GPUBufferHandle& indexBuffer )
            {
                _impl->SetIndexBuffer( indexBuffer );
            }

            void SetBindGroup( const GPUBindGroupHandle& bindGroup )
            {
                _impl->SetBindGroup( bindGroup );
            }

            void EndRenderPass()
            {
                _impl->EndRenderPass();
            }

            void Stop()
            {
                _impl->Stop();
            }

            [[nodiscard]] RenderEncoderImpl* GetImpl() { return _impl.get(); }

        private:
            std::unique_ptr<RenderEncoderImpl> _impl;
    };
}
