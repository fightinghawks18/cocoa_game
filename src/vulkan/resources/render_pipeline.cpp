#include "render_pipeline.h"
#include "../device.h"

namespace Cocoa::Vulkan {
    RenderPipeline::RenderPipeline(Device* device, vk::GraphicsPipelineCreateInfo desc) : _device(device) {
        _renderPipeline = device->GetDevice().createGraphicsPipelineUnique(nullptr, desc).value;
    }

    RenderPipeline::~RenderPipeline() {
        _device->GetDevice().waitIdle();
        _renderPipeline.reset();
    }
}