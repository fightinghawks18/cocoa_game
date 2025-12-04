#include "bind_group_layout.h"
#include "../device.h"

#include "../internal/helpers/enums.h"
#include "../internal/helpers/flags.h"

namespace Cocoa::Vulkan {
    BindGroupLayout::BindGroupLayout(Device* device, Graphics::GFXBindGroupLayoutDesc desc) : _device(device), _desc(desc) {
        std::vector<vk::DescriptorSetLayoutBinding> vkBindings;
        for (const auto& entry : desc.entries) {
            vk::DescriptorSetLayoutBinding binding;
            binding.binding = entry.binding;
            binding.descriptorCount = 1;
            binding.stageFlags = GPUShaderStageToVk(entry.visibility);
            binding.descriptorType = BindGroupTypeToVk(entry.type);
            vkBindings.push_back(binding);
        }
        
        vk::DescriptorSetLayoutCreateInfo layoutDescriptor{};
        layoutDescriptor.bindingCount = vkBindings.size();
        layoutDescriptor.pBindings = vkBindings.data();
        _layout = _device->GetDevice().createDescriptorSetLayoutUnique(layoutDescriptor);
    }

    BindGroupLayout::~BindGroupLayout() {
        _device->GetDevice().waitIdle();
        _layout.reset();
    }
}