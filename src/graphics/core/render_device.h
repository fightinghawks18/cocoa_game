#pragma once

#include <functional>
#include <memory>
#include <typeindex>

#include "../utils/descriptors.h"

#include "render_encoder.h"
#include "resource_manager.h"

namespace Cocoa::Graphics {
    using EncodeImmediateFun = std::function<void(RenderEncoder* encoder)>;

    class RenderDevice
    {
      public:
        virtual ~RenderDevice() = default;

        virtual RenderWindowHandle ConnectWindow(const RenderWindowDesc& desc) = 0;

        virtual GPUBufferHandle CreateBuffer(const GPUBufferDesc& desc) = 0;

        virtual GPUTextureHandle CreateTexture(const GPUTextureDesc& desc) = 0;

        virtual GPUTextureViewHandle CreateTextureView(const GPUTextureViewDesc& desc) = 0;

        virtual GPUSamplerHandle CreateSampler(const GPUSamplerDesc& desc) = 0;

        virtual GPUBindGroupHandle CreateBindGroup(const GPUBindGroupDesc& desc) = 0;

        virtual GPUBindGroupLayoutHandle CreateBindGroupLayout(const GPUBindGroupLayoutDesc& desc) = 0;

        virtual GFXRenderPipelineHandle CreateRenderPipeline(const GFXPipelineDesc& desc) = 0;

        virtual GFXPipelineLayoutHandle CreatePipelineLayout(const GFXPipelineLayoutDesc& desc) = 0;

        virtual GFXShaderModuleHandle CreateShaderModule(const GFXShaderModuleDesc& desc) = 0;

        virtual void DisconnectWindow(RenderWindowHandle& handle) = 0;

        virtual void DestroyBuffer(GPUBufferHandle& handle) = 0;

        virtual void DestroyTexture(GPUTextureHandle& handle) = 0;

        virtual void DestroyTextureView(GPUTextureViewHandle& handle) = 0;

        virtual void DestroySampler(GPUSamplerHandle& handle) = 0;

        virtual void DestroyBindGroup(GPUBindGroupHandle& handle) = 0;

        virtual void DestroyBindGroupLayout(GPUBindGroupLayoutHandle& handle) = 0;

        virtual void DestroyRenderPipeline(GFXRenderPipelineHandle& handle) = 0;

        virtual void DestroyPipelineLayout(GFXPipelineLayoutHandle& handle) = 0;

        virtual void DestroyShaderModule(GFXShaderModuleHandle& handle) = 0;

        virtual void WaitForIdle() = 0;

        virtual std::unique_ptr<RenderEncoder> Encode(const RenderEncoderDesc& encoderDesc) = 0;

        virtual void EndEncoding(std::unique_ptr<RenderEncoder> encoder) = 0;

        virtual void EncodeImmediateCommands(EncodeImmediateFun encodeFun, const RenderEncoderDesc& encoderDesc) = 0;

        template <typename T> [[nodiscard]] T* As() { return static_cast<T*>(this); }

      protected:
        std::unordered_map<std::type_index, std::unique_ptr<IResourceManager>> _resourceManagers;

        template <typename T, typename DescType> Handle* CreateResource(const DescType& desc)
        {
            ResourceManager<T>* manager = GetManager<T>();
            if (!manager)
                return nullptr;
            return manager->Create(desc);
        }

        template <typename T> void DestroyResource(Handle& handle)
        {
            ResourceManager<T>* manager = GetManager<T>();
            if (!manager)
                return;
            return manager->Destroy(handle);
        }

        template <typename T> [[nodiscard]] T* ResolveResource(Handle& handle)
        {
            ResourceManager<T>* manager = GetManager<T>();
            if (!manager)
                return nullptr;
            return manager->Get(handle);
        }

        template <typename T> void AddManager()
        {
            const auto& typeId = typeid(T);
            if (_resourceManagers.contains(typeId))
                return;
            _resourceManagers[typeId] = std::make_unique<ResourceManager<T>>();
        }

        template <typename T> [[nodiscard]] ResourceManager<T>* GetManager()
        {
            const auto& typeId = typeid(T);
            if (!_resourceManagers.contains(typeId))
                return nullptr;
            return static_cast<ResourceManager<T>*>(_resourceManagers[typeId].get());
        }
    };
} // namespace Cocoa::Graphics
