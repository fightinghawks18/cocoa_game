#pragma once

#include <vulkan/vulkan.hpp>

#include "../../graphics/descriptors.h"

namespace Cocoa::Vulkan {
    class Device;
    class BindGroup {
    public:
        BindGroup(Device* device, Graphics::BindGroupDesc desc);
        ~BindGroup() = default;

        BindGroup(const BindGroup& other) = delete;
        BindGroup(BindGroup&& other) noexcept = default;
        BindGroup& operator=(const BindGroup& other) = delete;
        BindGroup& operator=(BindGroup&& other) noexcept = default;

        [[nodiscard]] vk::DescriptorSet GetBinding() { return _set.get(); }
    private:
        Device* _device;

        vk::UniqueDescriptorSet _set;
    };
}