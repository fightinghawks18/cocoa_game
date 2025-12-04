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
        GFXDeviceImpl(GFXDevice& device) : _device(device) {}
        virtual ~GFXDeviceImpl() {}
        virtual void RegisterResources() = 0;
        virtual GFXEncoder Encode(GFXEncoderDesc encoderDesc) = 0;
        virtual void EndEncoding(GFXEncoder& encoder) = 0;
        virtual void EncodeImmediateCommands(EncodeImmediateFun encodeFun, GFXEncoderDesc encoderDesc) = 0;
    protected:
        GFXDevice& _device;
    };

    class GFXDevice {
    public:
        static std::unique_ptr<GFXDevice> Create();

        ~GFXDevice();

        template <typename T, typename DescType>
        Handle<T>* CreateResource(const DescType& desc) {
            GFXResourceManager<T>* manager = GetManager<T>();
            if (!manager) return nullptr;
            return manager->Create(desc);
        }

        template <typename T>
        void DestroyResource(Handle<T>& handle) {
            GFXResourceManager<T>* manager = GetManager<T>();
            if (!manager) return;
            return manager->Destroy(handle);
        }
        
        template <typename ResType, typename T>
        [[nodiscard]] ResType* ResolveResource(Handle<T>& handle) {
            GFXResourceManager<T>* manager = GetManager<T>();
            if (!manager) return nullptr;
            return manager->Get(handle);
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

        template <typename T>
        [[nodiscard]] GFXResourceManager<T>* GetManager() {
            const auto& typeId = typeid(T);
            if (!_resourceManagers.contains(typeId)) return nullptr;
            return static_cast<GFXResourceManager<T>*>(_resourceManagers[typeId].get());
        }

        GFXDevice();
        
    };
}