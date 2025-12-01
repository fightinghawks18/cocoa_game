#pragma once

#include <vulkan/vulkan.hpp>
#include "../../../graphics/enums.h"

namespace Cocoa::Vulkan {
    inline vk::AttachmentLoadOp GPUPassLoadOpToVk(Graphics::GPUPassLoadOp op) {
        switch (op) {
            case Graphics::GPUPassLoadOp::Clear:
                return vk::AttachmentLoadOp::eClear;
            case Graphics::GPUPassLoadOp::Load:
                return vk::AttachmentLoadOp::eLoad;
            case Graphics::GPUPassLoadOp::DontCare:
                return vk::AttachmentLoadOp::eDontCare;
        }
    }

    inline vk::AttachmentStoreOp GPUPassStoreOpToVk(Graphics::GPUPassStoreOp op) {
        switch (op) {
            case Graphics::GPUPassStoreOp::Store:
                return vk::AttachmentStoreOp::eStore;
            case Graphics::GPUPassStoreOp::DontCare:
                return vk::AttachmentStoreOp::eDontCare;
        }
    }

    inline vk::DescriptorType BindGroupTypeToVk(Graphics::BindGroupType type) {
        switch (type) {
            case Graphics::BindGroupType::UniformBuffer:
                return vk::DescriptorType::eUniformBuffer;
            case Graphics::BindGroupType::StorageBuffer:
                return vk::DescriptorType::eStorageBuffer;
            case Graphics::BindGroupType::Texture:
                return vk::DescriptorType::eUniformTexelBuffer;
            case Graphics::BindGroupType::Sampler:
                return vk::DescriptorType::eSampler;
        }
    }
}