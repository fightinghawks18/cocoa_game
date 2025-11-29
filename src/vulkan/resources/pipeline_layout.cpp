#include "pipeline_layout.h"
#include "../device.h"

namespace Cocoa::Vulkan {
    PipelineLayout::PipelineLayout(Device* device, vk::PipelineLayoutCreateInfo desc) : _device(device) {
        _pipelineLayout = device->GetDevice().createPipelineLayoutUnique(desc);
    }

    PipelineLayout::~PipelineLayout() {
        _device->GetDevice().waitIdle();
        _pipelineLayout.reset();
    }
}