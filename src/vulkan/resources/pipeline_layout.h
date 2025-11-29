#pragma once

#include <vulkan/vulkan.hpp>

namespace Cocoa::Vulkan {
    class Device;
    class PipelineLayout {
    public:
        PipelineLayout(Device* device, vk::PipelineLayoutCreateInfo desc);
        ~PipelineLayout();

        [[nodiscard]] vk::PipelineLayout Get() { return _pipelineLayout.get(); }
    private:
        Device* _device;

        vk::UniquePipelineLayout _pipelineLayout;
    };
}