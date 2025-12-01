#include "bind_group.h"
#include "../device.h"

#include "../internal/helpers/enums.h"
#include "../internal/helpers/flags.h"
namespace Cocoa::Vulkan {
    BindGroup::BindGroup(Device* device, Graphics::BindGroupDesc desc) : _device(device) {
        auto groupLayoutInstance = device->GetBindGroupLayoutInstance(desc.layout);
        auto layouts = { groupLayoutInstance->GetLayout() };

        vk::DescriptorSetAllocateInfo descriptorSetDescriptor{};
        descriptorSetDescriptor.setDescriptorPool(device->GetDescriptorPool())
                    .setDescriptorSetCount(1)
                    .setSetLayouts(layouts);

        auto sets = _device->GetDevice().allocateDescriptorSetsUnique(descriptorSetDescriptor);
        _set = std::move(sets[0]);

        std::vector<vk::WriteDescriptorSet> writes;
        std::vector<vk::DescriptorBufferInfo> bufferDescriptors;
        std::vector<vk::DescriptorImageInfo> imageDescriptors;

        writes.reserve(desc.entries.size());
        bufferDescriptors.reserve(desc.entries.size());
        imageDescriptors.reserve(desc.entries.size());

        for (uint32_t i = 0; i < desc.entries.size(); i++) {
            const auto layoutEntry = groupLayoutInstance->GetDesc().entries[i];
            const auto& resource = desc.entries[i];

            vk::WriteDescriptorSet descriptorWrite{};
            descriptorWrite.setDstSet(_set.get())
                        .setDstBinding(layoutEntry.binding)
                        .setDescriptorCount(1);
            
            
            switch (layoutEntry.type) {
                case Graphics::GPUBindGroupType::UniformBuffer:
                case Graphics::GPUBindGroupType::StorageBuffer: {
                    auto bufferHandle = std::get<Graphics::BufferHandle>(resource);
                    auto bufferInstance = device->GetBufferInstance(bufferHandle);

                    vk::DescriptorBufferInfo bufferInfo;
                    bufferInfo.buffer = bufferInstance->Get();
                    bufferInfo.offset = 0;
                    bufferInfo.range = bufferInstance->GetSize();
                    bufferDescriptors.push_back(bufferInfo);

                    descriptorWrite.descriptorType = (layoutEntry.type == Graphics::GPUBindGroupType::UniformBuffer) 
                        ? vk::DescriptorType::eUniformBuffer 
                        : vk::DescriptorType::eStorageBuffer;
                    descriptorWrite.pBufferInfo = &bufferDescriptors.back();
                    break;
                }
                case Graphics::GPUBindGroupType::Texture: {
                    auto textureHandle = std::get<Graphics::TextureHandle>(resource);
                    auto image = device->GetTextureInstance(textureHandle);
                    auto imageView = image->GetView(0);
                    vk::DescriptorImageInfo imageDescriptor{};
                    imageDescriptor.setImageView(imageView->Get())
                                .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
                    imageDescriptors.push_back(imageDescriptor);

                    descriptorWrite.descriptorType = vk::DescriptorType::eSampledImage;
                    descriptorWrite.pImageInfo = &imageDescriptors.back();
                    break;
                }
                case Graphics::GPUBindGroupType::Sampler: {
                    auto samplerHandle = std::get<Graphics::SamplerHandle>(resource);
                    auto sampler = device->GetSamplerInstance(samplerHandle);
                    auto samplerObject = sampler->Get();
                    vk::DescriptorImageInfo imageDescriptor{};
                    imageDescriptor.setSampler(samplerObject);
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