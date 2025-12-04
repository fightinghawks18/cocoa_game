#pragma once

#include <vulkan/vulkan.hpp>

#include "../../graphics/descriptors.h"

namespace Cocoa::Vulkan {
    class Device;
    class BindGroupLayout {
    public:
        BindGroupLayout(Device* device, Graphics::GFXBindGroupLayoutDesc desc);
        ~BindGroupLayout();

        BindGroupLayout(const BindGroupLayout& other) = delete;
        BindGroupLayout(BindGroupLayout&& other) noexcept = default;
        BindGroupLayout& operator=(const BindGroupLayout& other) = delete;
        BindGroupLayout& operator=(BindGroupLayout&& other) noexcept = default;

        [[nodiscard]] vk::DescriptorSetLayout GetLayout() { return _layout.get(); }
        [[nodiscard]] Graphics::GFXBindGroupLayoutDesc GetDesc() { return _desc; }
    private:
        Device* _device;

        vk::UniqueDescriptorSetLayout _layout;
        Graphics::GFXBindGroupLayoutDesc _desc;
    };
}