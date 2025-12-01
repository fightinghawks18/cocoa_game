#include "pipeline_layout.h"
#include "../device.h"

namespace Cocoa::Vulkan {
    PipelineLayout::PipelineLayout(Device* device, Graphics::PipelineLayoutDesc desc) : _device(device) {
        std::vector<vk::DescriptorSetLayout> setLayouts;
        setLayouts.reserve(desc.groupLayouts.size());
        for (const auto& groupLayoutHandle : desc.groupLayouts) {
            auto groupLayout = _device->GetBindGroupLayoutInstance(groupLayoutHandle);
            setLayouts.push_back(groupLayout->GetLayout());
        }

        vk::PipelineLayoutCreateInfo pipelineLayoutDescriptor;
        pipelineLayoutDescriptor.setSetLayouts(setLayouts)
                    .setPushConstantRanges(nullptr);
        _pipelineLayout = device->GetDevice().createPipelineLayoutUnique(pipelineLayoutDescriptor);
    }

    PipelineLayout::~PipelineLayout() {
        _device->GetDevice().waitIdle();
        _pipelineLayout.reset();
    }
}