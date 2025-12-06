#pragma once

#include <functional>
#include <memory>
#include <typeindex>

#include "../utils/descriptors.h"

#include "encoder.h"
#include "resource_manager.h"

namespace Cocoa::Graphics
{
    using EncodeImmediateFun = std::function<void( RenderEncoder& encoder )>;

    class RenderDevice;

    struct RenderDeviceImpl
    {
        virtual ~RenderDeviceImpl() = default;

        virtual void RegisterResources( RenderDevice& device ) = 0;

        virtual RenderWindowHandle ConnectWindow( const RenderWindowDesc& desc ) = 0;

        virtual GPUBufferHandle CreateBuffer( const GPUBufferDesc& desc ) = 0;

        virtual GPUTextureHandle CreateTexture( const GPUTextureDesc& desc ) = 0;

        virtual GPUTextureViewHandle CreateTextureView( const GPUTextureViewDesc& desc ) = 0;

        virtual GPUSamplerHandle CreateSampler( const GPUSamplerDesc& desc ) = 0;

        virtual GPUBindGroupHandle CreateBindGroup( const GPUBindGroupDesc& desc ) = 0;

        virtual GPUBindGroupLayoutHandle CreateBindGroupLayout( const GPUBindGroupLayoutDesc& desc ) = 0;

        virtual GFXRenderPipelineHandle CreateRenderPipeline( const GFXPipelineDesc& desc ) = 0;

        virtual GFXPipelineLayoutHandle CreatePipelineLayout( const GFXPipelineLayoutDesc& desc ) = 0;

        virtual GFXShaderModuleHandle CreateShaderModule( const GFXShaderModuleDesc& desc ) = 0;

        virtual void DisconnectWindow( RenderWindowHandle& handle ) = 0;

        virtual void DestroyBuffer( GPUBufferHandle& handle ) = 0;

        virtual void DestroyTexture( GPUTextureHandle& handle ) = 0;

        virtual void DestroyTextureView( GPUTextureViewHandle& handle ) = 0;

        virtual void DestroySampler( GPUSamplerHandle& handle ) = 0;

        virtual void DestroyBindGroup( GPUBindGroupHandle& handle ) = 0;

        virtual void DestroyBindGroupLayout( GPUBindGroupLayoutHandle& handle ) = 0;

        virtual void DestroyRenderPipeline( GFXRenderPipelineHandle& handle ) = 0;

        virtual void DestroyPipelineLayout( GFXPipelineLayoutHandle& handle ) = 0;

        virtual void DestroyShaderModule( GFXShaderModuleHandle& handle ) = 0;

        virtual void* GetWindowImpl( RenderWindowHandle& handle ) = 0;

        virtual void* GetBufferImpl( GPUBufferHandle& handle ) = 0;

        virtual void* GetTextureImpl( GPUTextureHandle& handle ) = 0;

        virtual void* GetTextureViewImpl( GPUTextureViewHandle& handle ) = 0;

        virtual void* GetSamplerImpl( GPUSamplerHandle& handle ) = 0;

        virtual void* GetBindGroupImpl( GPUBindGroupHandle& handle ) = 0;

        virtual void* GetBindGroupLayoutImpl( GPUBindGroupLayoutHandle& handle ) = 0;

        virtual void* GetRenderPipelineImpl( GFXRenderPipelineHandle& handle ) = 0;

        virtual void* GetPipelineLayoutImpl( GFXPipelineLayoutHandle& handle ) = 0;

        virtual void* GetShaderModuleImpl( GFXShaderModuleHandle& handle ) = 0;

        virtual void WaitForIdle() = 0;

        virtual RenderEncoder Encode( const RenderEncoderDesc& encoderDesc ) = 0;

        virtual void EndEncoding( RenderEncoder& encoder ) = 0;

        virtual void EncodeImmediateCommands( EncodeImmediateFun encodeFun, const RenderEncoderDesc& encoderDesc ) = 0;

        protected:
            std::unordered_map<std::type_index, std::unique_ptr<IResourceManager>> _resourceManagers;

            template<typename T, typename DescType>
            Handle* CreateResource( const DescType& desc )
            {
                ResourceManager<T>* manager = GetManager<T>();
                if ( !manager ) return nullptr;
                return manager->Create( desc );
            }

            template<typename T>
            void DestroyResource( Handle& handle )
            {
                ResourceManager<T>* manager = GetManager<T>();
                if ( !manager ) return;
                return manager->Destroy( handle );
            }

            template<typename T>
            [[nodiscard]] T* ResolveResource( Handle& handle )
            {
                ResourceManager<T>* manager = GetManager<T>();
                if ( !manager ) return nullptr;
                return manager->Get( handle );
            }

            template<typename T>
            void AddManager()
            {
                const auto& typeId = typeid( T );
                if ( _resourceManagers.contains( typeId ) ) return;
                _resourceManagers[ typeId ] = std::make_unique<ResourceManager<T>>();
            }

            template<typename T>
            [[nodiscard]] ResourceManager<T>* GetManager()
            {
                const auto& typeId = typeid( T );
                if ( !_resourceManagers.contains( typeId ) ) return nullptr;
                return static_cast<ResourceManager<T>*>( _resourceManagers[ typeId ].get() );
            }
    };

    class RenderDevice
    {
        public:
            explicit RenderDevice( std::unique_ptr<RenderDeviceImpl> impl ) : _impl( std::move( impl ) )
            {
                _impl->RegisterResources( *this );
            }

            ~RenderDevice()
            {
                _impl.reset();
            }

            RenderWindowHandle ConnectWindow( const RenderWindowDesc& desc )
            {
                return _impl->ConnectWindow( desc );
            }

            GPUBufferHandle CreateBuffer( const GPUBufferDesc& desc )
            {
                return _impl->CreateBuffer( desc );
            }

            GPUTextureHandle CreateTexture( const GPUTextureDesc& desc )
            {
                return _impl->CreateTexture( desc );
            }

            GPUTextureViewHandle CreateTextureView( const GPUTextureViewDesc& desc )
            {
                return _impl->CreateTextureView( desc );
            }

            GPUSamplerHandle CreateSampler( const GPUSamplerDesc& desc )
            {
                return _impl->CreateSampler( desc );
            }

            GPUBindGroupHandle CreateBindGroup( const GPUBindGroupDesc& desc )
            {
                return _impl->CreateBindGroup( desc );
            }

            GPUBindGroupLayoutHandle CreateBindGroupLayout( const GPUBindGroupLayoutDesc& desc )
            {
                return _impl->CreateBindGroupLayout( desc );
            }

            GFXRenderPipelineHandle CreateRenderPipeline( const GFXPipelineDesc& desc )
            {
                return _impl->CreateRenderPipeline( desc );
            }

            GFXPipelineLayoutHandle CreatePipelineLayout( const GFXPipelineLayoutDesc& desc )
            {
                return _impl->CreatePipelineLayout( desc );
            }

            GFXShaderModuleHandle CreateShaderModule( const GFXShaderModuleDesc& desc )
            {
                return _impl->CreateShaderModule( desc );
            }

            void DisconnectWindow( RenderWindowHandle& handle )
            {
                _impl->DisconnectWindow( handle );
            }

            void DestroyBuffer( GPUBufferHandle& handle )
            {
                _impl->DestroyBuffer( handle );
            }

            void DestroyTexture( GPUTextureHandle& handle )
            {
                _impl->DestroyTexture( handle );
            }

            void DestroyTextureView( GPUTextureViewHandle& handle )
            {
                _impl->DestroyTextureView( handle );
            }

            void DestroySampler( GPUSamplerHandle& handle )
            {
                _impl->DestroySampler( handle );
            }

            void DestroyBindGroup( GPUBindGroupHandle& handle )
            {
                _impl->DestroyBindGroup( handle );
            }

            void DestroyBindGroupLayout( GPUBindGroupLayoutHandle& handle )
            {
                _impl->DestroyBindGroupLayout( handle );
            }

            void DestroyRenderPipeline( GFXRenderPipelineHandle& handle )
            {
                _impl->DestroyRenderPipeline( handle );
            }

            void DestroyPipelineLayout( GFXPipelineLayoutHandle& handle )
            {
                _impl->DestroyPipelineLayout( handle );
            }

            void DestroyShaderModule( GFXShaderModuleHandle& handle )
            {
                _impl->DestroyShaderModule( handle );
            }

            void WaitForIdle()
            {
                _impl->WaitForIdle();
            }

            [[nodiscard]] RenderEncoder Encode( const RenderEncoderDesc& encoderDesc ) const
            {
                return _impl->Encode( encoderDesc );
            }

            void EndEncoding( RenderEncoder& encoder )
            {
                _impl->EndEncoding( encoder );
            }

            void EncodeImmediateCommands( const EncodeImmediateFun& encodeFun, const RenderEncoderDesc& encoderDesc )
            {
                _impl->EncodeImmediateCommands( encodeFun , encoderDesc );
            }

            [[nodiscard]] RenderDeviceImpl* GetImpl() { return _impl.get(); }

        private:
            std::unique_ptr<RenderDeviceImpl> _impl;

            friend struct RenderDeviceImpl;
    };
}
