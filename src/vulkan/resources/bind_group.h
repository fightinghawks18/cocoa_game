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

        [[nodiscard]] vk::DescriptorSetLayout GetLayout() { return _layout.get(); }
        [[nodiscard]] vk::DescriptorSet GetBinding() { return _set.get(); }
    private:
        Device* _device;

        vk::UniqueDescriptorSetLayout _layout;
        vk::UniqueDescriptorSet _set;
    };
}