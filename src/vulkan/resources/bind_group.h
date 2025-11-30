#pragma once

#include <cstdint>
#include "../common.h"
#include "../handles.h"

namespace Cocoa::Vulkan {
    enum class BindGroupType {
        UniformBuffer,
        StorageBuffer,
        Texture,
        Sampler
    };

    struct BindGroupLayoutEntry {
        uint32_t binding;
        GPUShaderStage visibility;
        BindGroupType type;
    };

    struct BindGroupBuffer {
        BufferHandle buffer;
        vk::DeviceSize offset;
        vk::DeviceSize size;
    };

    struct BindGroupTexture {
        TextureHandle texture;
    };

    struct BindGroupSampler {
        SamplerHandle sampler;
    };

    struct BindGroupEntry {
        uint32_t binding;
        union {
            BindGroupBuffer buffer;
            BindGroupTexture texture;
            BindGroupSampler sampler;
        };
    };

    struct BindGroupLayoutDesc {
        std::vector<BindGroupLayoutEntry> entries;
    };

    struct BindGroupDesc {
        BindGroupLayoutDesc* layout;
        std::vector<BindGroupEntry> entries;
    };

    inline vk::DescriptorType BindGroupTypeToVk(BindGroupType type) {
        switch (type) {
            case BindGroupType::UniformBuffer:
                return vk::DescriptorType::eUniformBuffer;
            case BindGroupType::StorageBuffer:
                return vk::DescriptorType::eStorageBuffer;
            case BindGroupType::Texture:
                return vk::DescriptorType::eUniformTexelBuffer;
            case BindGroupType::Sampler:
                return vk::DescriptorType::eSampler;
        }
    }

    class Device;
    class BindGroup {
    public:
        BindGroup(Device* device, BindGroupDesc desc);
        ~BindGroup() = default;

        BindGroup(const BindGroup& other) = delete;
        BindGroup(BindGroup&& other) noexcept = default;
        BindGroup& operator=(const BindGroup& other) = delete;
        BindGroup& operator=(BindGroup&& other) noexcept = default;

        [[nodiscard]] vk::DescriptorSetLayout GetLayout() { return _layout.get(); }
        [[nodiscard]] vk::DescriptorSet GetBinding() { return _set.get(); }
    private:
        Device* _device;

        vk::UniqueDescriptorSetLayout _layout;
        vk::UniqueDescriptorSet _set;
    };
}