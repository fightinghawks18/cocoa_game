#pragma once

#include <vulkan/vulkan.hpp>
#include "../graphics/enums.h"
#include "../graphics/flags.h"

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

    inline vk::BufferUsageFlags GPUBufferUsageToVk(Graphics::GPUBufferUsage stage) {
        vk::BufferUsageFlags flags;
        if (stage & Graphics::GPUBufferUsage::Vertex) flags |= vk::BufferUsageFlagBits::eVertexBuffer;
        if (stage & Graphics::GPUBufferUsage::Index) flags |= vk::BufferUsageFlagBits::eIndexBuffer;
        if (stage & Graphics::GPUBufferUsage::Storage) flags |= vk::BufferUsageFlagBits::eStorageBuffer;
        if (stage & Graphics::GPUBufferUsage::Uniform) flags |= vk::BufferUsageFlagBits::eUniformBuffer;
        return flags;
    }

    inline vk::ShaderStageFlags GPUShaderStageToVk(Graphics::GPUShaderStage stage) {
        vk::ShaderStageFlags flags;
        if (stage & Graphics::GPUShaderStage::Vertex) flags |= vk::ShaderStageFlagBits::eVertex;
        if (stage & Graphics::GPUShaderStage::Pixel) flags |= vk::ShaderStageFlagBits::eFragment;
        return flags;
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