#pragma once

#include <vulkan/vulkan.hpp>

namespace Cocoa::Vulkan {
    class Device;
    class PipelineLayout {
    public:
        PipelineLayout(Device* device, vk::PipelineLayoutCreateInfo desc);
        ~PipelineLayout();

        PipelineLayout(const PipelineLayout& other) = delete;
        PipelineLayout(PipelineLayout&& other) noexcept = default;
        PipelineLayout& operator=(const PipelineLayout& other) = delete;
        PipelineLayout& operator=(PipelineLayout&& other) noexcept = default;

        [[nodiscard]] vk::PipelineLayout Get() { return _pipelineLayout.get(); }
    private:
        Device* _device;

        vk::UniquePipelineLayout _pipelineLayout;
    };
}