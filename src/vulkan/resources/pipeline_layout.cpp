#include "pipeline_layout.h"
#include "../device.h"

namespace Cocoa::Vulkan {
    PipelineLayout::PipelineLayout(Device* device, Graphics::PipelineLayoutDesc desc) : _device(device) {
        std::vector<vk::DescriptorSetLayout> setLayouts;
        setLayouts.reserve(desc.bindGroups.size());
        for (const auto& bindGroupHandle : desc.bindGroups) {
            auto bindGroupInstance = _device->GetBindGroupInstance(bindGroupHandle);
            setLayouts.push_back(bindGroupInstance->GetLayout());
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