#include "bind_group.h"
#include "../device.h"

#include "../helpers.h"
namespace Cocoa::Vulkan {
    BindGroup::BindGroup(Device* device, Graphics::BindGroupDesc desc) : _device(device) {
        std::vector<vk::DescriptorSetLayoutBinding> vkBindings;
        for (const auto& entry : desc.layout->entries) {
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

        vk::DescriptorSetAllocateInfo descriptorSetDescriptor{};
        descriptorSetDescriptor.setDescriptorPool(device->GetDescriptorPool())
                    .setDescriptorSetCount(1)
                    .setSetLayouts(_layout.get());

        auto sets = _device->GetDevice().allocateDescriptorSetsUnique(descriptorSetDescriptor);
        _set = std::move(sets[0]);

        std::vector<vk::WriteDescriptorSet> writes;
        std::vector<vk::DescriptorBufferInfo> bufferDescriptors;
        std::vector<vk::DescriptorImageInfo> imageDescriptors;

        for (const auto& entry : desc.entries) {
            vk::WriteDescriptorSet descriptorWrite{};
            descriptorWrite.setDstSet(_set.get())
                        .setDstBinding(entry.binding)
                        .setDescriptorCount(1);
 
            Graphics::BindGroupLayoutEntry layoutEntry;
            for (const auto& lEntry : desc.layout->entries) {
                if (lEntry.binding != entry.binding) continue;
                layoutEntry = lEntry;
            }
            
            switch (layoutEntry.type) {
                case Graphics::BindGroupType::UniformBuffer:
                case Graphics::BindGroupType::StorageBuffer: {
                    auto bufferInstance = device->GetBufferInstance(entry.buffer);

                    vk::DescriptorBufferInfo bufferInfo;
                    bufferInfo.buffer = bufferInstance->Get();
                    bufferInfo.offset = 0;
                    bufferInfo.range = bufferInstance->GetSize();
                    bufferDescriptors.push_back(bufferInfo);

                    descriptorWrite.descriptorType = (layoutEntry.type == Graphics::BindGroupType::UniformBuffer) 
                        ? vk::DescriptorType::eUniformBuffer 
                        : vk::DescriptorType::eStorageBuffer;
                    descriptorWrite.pBufferInfo = &bufferDescriptors.back();
                    break;
                }
                case Graphics::BindGroupType::Texture: {
                    vk::DescriptorImageInfo imageDescriptor{};
                    imageDescriptor.setImageView(device->GetTextureInstance(entry.texture)->GetView())
                                .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
                    imageDescriptors.push_back(imageDescriptor);

                    descriptorWrite.descriptorType = vk::DescriptorType::eSampledImage;
                    descriptorWrite.pImageInfo = &imageDescriptors.back();
                    break;
                }
                case Graphics::BindGroupType::Sampler: {
                    vk::DescriptorImageInfo imageDescriptor{};
                    imageDescriptor.setSampler(device->GetSamplerInstance(entry.sampler)->Get());
                    imageDescriptors.push_back(imageDescriptor);

                    descriptorWrite.descriptorType = vk::DescriptorType::eSampler;
                    descriptorWrite.pImageInfo = &imageDescriptors.back();
                    break;
                }
            }

            writes.push_back(descriptorWrite);
        }

        _device->GetDevice().updateDescriptorSets(writes, nullptr);
    }
}