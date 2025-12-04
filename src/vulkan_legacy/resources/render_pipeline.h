#pragma once

#include <vulkan/vulkan.hpp>
#include "../../graphics/descriptors.h"

namespace Cocoa::Vulkan {
    class Device;
    class RenderPipeline {
    public:
        RenderPipeline(Device* device, Graphics::PipelineDesc desc);
        ~RenderPipeline();

        RenderPipeline(const RenderPipeline& other) = delete;
        RenderPipeline(RenderPipeline&& other) noexcept = default;
        RenderPipeline& operator=(const RenderPipeline& other) = delete;
        RenderPipeline& operator=(RenderPipeline&& other) noexcept = default;

        [[nodiscard]] vk::Pipeline Get() { return _renderPipeline.get(); }
    private:
        Device* _device;

        vk::UniquePipeline _renderPipeline;
    };
}