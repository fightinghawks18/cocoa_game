#pragma once

#include <memory>
#include <typeindex>

#include "../utils/descriptors.h"
#include "../utils/handles.h"

#include "resource_manager.h"

namespace Cocoa::Graphics {
    class GFXDevice {
    public:
        ~GFXDevice();

        template <typename T, typename DescType>
        Tools::Handle<T>* CreateResource(const DescType& desc) {
            GFXResourceManager<T>* manager = GetManager<T>();
            if (!manager) return nullptr;
            return manager->Create(desc);
        }

        template <typename T>
        void DestroyResource(Tools::Handle<T>& handle) {
            GFXResourceManager<T>* manager = GetManager<T>();
            if (!manager) return;
            return manager->Destroy(handle);
        }
        
        template <typename ResType, typename T>
        [[nodiscard]] ResType* ResolveResource(Tools::Handle<T>& handle) {
            GFXResourceManager<T>* manager = GetManager<T>();
            if (!manager) return nullptr;
            return manager->Get(handle);
        }

        //Encoder Encode(SwapchainHandle swapchain);
        //void EndEncoding(Encoder encoder);
        //void EncodeImmediateCommands(std::function<void(Encoder& encoder)> encoder, GPUQueueType queueType = GPUQueueType::Graphics);

        struct Impl;
        [[nodiscard]] Impl* GetImpl() { return _impl.get(); }
    private:
        std::unique_ptr<Impl> _impl;
        std::unordered_map<std::type_index, std::optional<IGFXResourceManager>> _resourceManagers;

        template <typename T>
        [[nodiscard]] GFXResourceManager<T>* GetManager() {
            const auto& typeId = typeid(T);
            if (!_resourceManagers.contains(typeId)) return nullptr;
            return static_cast<GFXResourceManager<T>>(_resourceManagers[typeId]);
        }

        GFXDevice();
        
    };
}