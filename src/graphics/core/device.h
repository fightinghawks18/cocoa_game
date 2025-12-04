#pragma once

#include <functional>
#include <memory>
#include <typeindex>

#include "../utils/descriptors.h"

#include "encoder.h"
#include "resource_manager.h"

namespace Cocoa::Graphics {
    using EncodeImmediateFun = std::function<void(GFXEncoder& encoder)>;

    class GFXDevice;
    struct GFXDeviceImpl {
        virtual ~GFXDeviceImpl() = default;
        virtual void RegisterResources(GFXDevice& device) = 0;
        virtual GFXWindowHandle ConnectWindow(const GFXWindowDesc& desc) = 0;
        virtual GPUBufferHandle CreateBuffer(const GPUBufferDesc& desc) = 0;
        virtual GPUTextureHandle CreateTexture(const GPUTextureDesc& desc) = 0;
        virtual GPUTextureViewHandle CreateTextureView(const GPUTextureViewDesc& desc) = 0;
        virtual GPUSamplerHandle CreateSampler(const GPUSamplerDesc& desc) = 0;
        virtual GPUBindGroupHandle CreateBindGroup(const GPUBindGroupDesc& desc) = 0;
        virtual GPUBindGroupLayoutHandle CreateBindGroupLayout(const GPUBindGroupLayoutDesc& desc) = 0;
        virtual GFXRenderPipelineHandle CreateRenderPipeline(const GFXPipelineDesc& desc) = 0;
        virtual GFXPipelineLayoutHandle CreatePipelineLayout(const GFXPipelineLayoutDesc& desc) = 0;
        virtual GFXShaderModuleHandle CreateShaderModule(const GFXShaderModuleDesc& desc) = 0;
        virtual void DisconnectWindow(GFXWindowHandle& handle) = 0;
        virtual void DestroyBuffer(GPUBufferHandle& handle) = 0;
        virtual void DestroyTexture(GPUTextureHandle& handle) = 0;
        virtual void DestroyTextureView(GPUTextureViewHandle& handle) = 0;
        virtual void DestroySampler(GPUSamplerHandle& handle) = 0;
        virtual void DestroyBindGroup(GPUBindGroupHandle& handle) = 0;
        virtual void DestroyBindGroupLayout(GPUBindGroupLayoutHandle& handle) = 0;
        virtual void DestroyRenderPipeline(GFXRenderPipelineHandle& handle) = 0;
        virtual void DestroyPipelineLayout(GFXPipelineLayoutHandle& handle) = 0;
        virtual void DestroyShaderModule(GFXShaderModuleHandle& handle) = 0;
        virtual GFXEncoder Encode(GFXEncoderDesc encoderDesc) = 0;
        virtual void EndEncoding(GFXEncoder& encoder) = 0;
        virtual void EncodeImmediateCommands(EncodeImmediateFun encodeFun, GFXEncoderDesc encoderDesc) = 0;
    };

    class GFXDevice {
    public:
        GFXDevice(std::unique_ptr<GFXDeviceImpl> impl) : _impl(std::move(impl)) {
            _impl->RegisterResources(*this);
        }

        ~GFXDevice() {
            _impl.reset();
            _resourceManagers.clear();
        }

        GFXWindowHandle ConnectWindow(const GFXWindowDesc& desc) {
            return _impl->ConnectWindow(desc);
        }

        GPUBufferHandle CreateBuffer(const GPUBufferDesc& desc) {
            return _impl->CreateBuffer(desc);
        }

        GPUTextureHandle CreateTexture(const GPUTextureDesc& desc) {
            return _impl->CreateTexture(desc);
        }

        GPUTextureViewHandle CreateTextureView(const GPUTextureViewDesc& desc) {
            return _impl->CreateTextureView(desc);
        }

        GPUSamplerHandle CreateSampler(const GPUSamplerDesc& desc) {
            return _impl->CreateSampler(desc);
        }

        GPUBindGroupHandle CreateBindGroup(const GPUBindGroupDesc& desc) {
            return _impl->CreateBindGroup(desc);
        }

        GPUBindGroupLayoutHandle CreateBindGroupLayout(const GPUBindGroupLayoutDesc& desc) {
            return _impl->CreateBindGroupLayout(desc);
        }

        GFXRenderPipelineHandle CreateRenderPipeline(const GFXPipelineDesc& desc) {
            return _impl->CreateRenderPipeline(desc);
        }

        GFXPipelineLayoutHandle CreatePipelineLayout(const GFXPipelineLayoutDesc& desc) {
            return _impl->CreatePipelineLayout(desc);
        }

        GFXShaderModuleHandle CreateShaderModule(const GFXShaderModuleDesc& desc) {
            return _impl->CreateShaderModule(desc);
        }

        void DisconnectWindow(GFXWindowHandle& handle) {
            _impl->DisconnectWindow(handle);
        }

        void DestroyBuffer(GPUBufferHandle& handle) {
            _impl->DestroyBuffer(handle);
        }

        void DestroyTexture(GPUTextureHandle& handle) {
            _impl->DestroyTexture(handle);
        }

        void DestroyTextureView(GPUTextureViewHandle& handle) {
            _impl->DestroyTextureView(handle);
        }

        void DestroySampler(GPUSamplerHandle& handle) {
            _impl->DestroySampler(handle);
        }

        void DestroyBindGroup(GPUBindGroupHandle& handle) {
            _impl->DestroyBindGroup(handle);
        }

        void DestroyBindGroupLayout(GPUBindGroupLayoutHandle& handle) {
            _impl->DestroyBindGroupLayout(handle);
        }

        void DestroyRenderPipeline(GFXRenderPipelineHandle& handle) {
            _impl->DestroyRenderPipeline(handle);
        }

        void DestroyPipelineLayout(GFXPipelineLayoutHandle& handle) {
            _impl->DestroyPipelineLayout(handle);
        }

        void DestroyShaderModule(GFXShaderModuleHandle& handle) {
            _impl->DestroyShaderModule(handle);
        }

        GFXEncoder Encode(GFXEncoderDesc encoderDesc) {
            return _impl->Encode(encoderDesc);
        }

        void EndEncoding(GFXEncoder& encoder) {
            _impl->EndEncoding(encoder);
        }

        void EncodeImmediateCommands(EncodeImmediateFun encodeFun, GFXEncoderDesc encoderDesc) {
            _impl->EncodeImmediateCommands(encodeFun, encoderDesc);
        }

        [[nodiscard]] GFXDeviceImpl* GetImpl() { return _impl.get(); }
    private:
        std::unique_ptr<GFXDeviceImpl> _impl;
        std::unordered_map<std::type_index, std::unique_ptr<IGFXResourceManager>> _resourceManagers;

        template <typename T, typename DescType>
        Handle* CreateResource(const DescType& desc) {
            GFXResourceManager<T>* manager = GetManager<T>();
            if (!manager) return nullptr;
            return manager->Create(desc);
        }

        template <typename T>
        void DestroyResource(Handle& handle) {
            GFXResourceManager<T>* manager = GetManager<T>();
            if (!manager) return;
            return manager->Destroy(handle);
        }
        
        template <typename ResType, typename T>
        [[nodiscard]] ResType* ResolveResource(Handle& handle) {
            GFXResourceManager<T>* manager = GetManager<T>();
            if (!manager) return nullptr;
            return manager->Get(handle);
        }

        template <typename T>
        void AddManager() {
            const auto& typeId = typeid(T);
            if (_resourceManagers.contains(typeId)) return;
            _resourceManagers[typeId] = std::make_unique<T>();
        }

        template <typename T>
        [[nodiscard]] GFXResourceManager<T>* GetManager() {
            const auto& typeId = typeid(T);
            if (!_resourceManagers.contains(typeId)) return nullptr;
            return static_cast<GFXResourceManager<T>*>(_resourceManagers[typeId].get());
        }

        friend struct GFXDeviceImpl;
    };
}