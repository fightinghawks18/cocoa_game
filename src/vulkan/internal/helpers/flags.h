#pragma once

#include <vulkan/vulkan.hpp>
#include "../../../graphics/flags.h"

namespace Cocoa::Vulkan {
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
}