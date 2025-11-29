#pragma once

#include <vulkan/vulkan.hpp>

namespace Cocoa::Vulkan {
    class Device;
    class RenderPipeline {
    public:
        RenderPipeline(Device* device, vk::GraphicsPipelineCreateInfo desc);
        ~RenderPipeline();

        [[nodiscard]] vk::Pipeline Get() { return _renderPipeline.get(); }
    private:
        Device* _device;

        vk::UniquePipeline _renderPipeline;
    };
}