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
        if (stage & Graphics::GPUBufferUsage::TransferSrc) flags |= vk::BufferUsageFlagBits::eTransferSrc;
        if (stage & Graphics::GPUBufferUsage::TransferDst) flags |= vk::BufferUsageFlagBits::eTransferDst;
        return flags;
    }

    inline vk::ShaderStageFlags GPUShaderStageToVk(Graphics::GPUShaderStage stage) {
        vk::ShaderStageFlags flags;
        if (stage & Graphics::GPUShaderStage::Vertex) flags |= vk::ShaderStageFlagBits::eVertex;
        if (stage & Graphics::GPUShaderStage::Pixel) flags |= vk::ShaderStageFlagBits::eFragment;
        return flags;
    }

    inline vk::ShaderStageFlagBits GPUShaderStageToVkBit(Graphics::GPUShaderStage stage) {
        if (stage & Graphics::GPUShaderStage::Vertex) return vk::ShaderStageFlagBits::eVertex;
        if (stage & Graphics::GPUShaderStage::Pixel) return vk::ShaderStageFlagBits::eFragment;
        return vk::ShaderStageFlagBits::eAll;
    }

    inline vk::ImageAspectFlags GPUTextureAspectToVk(Graphics::GPUTextureAspect aspect) {
        vk::ImageAspectFlags flags;
        if (aspect & Graphics::GPUTextureAspect::None) flags |= vk::ImageAspectFlagBits::eNone;
        if (aspect & Graphics::GPUTextureAspect::Color) flags |= vk::ImageAspectFlagBits::eColor;
        if (aspect & Graphics::GPUTextureAspect::Depth) flags |= vk::ImageAspectFlagBits::eDepth;
        if (aspect & Graphics::GPUTextureAspect::Stencil) flags |= vk::ImageAspectFlagBits::eStencil;
        return flags;
    }

    inline vk::ImageUsageFlags GPUTextureUsageToVk(Graphics::GPUTextureUsage usage) {
        vk::ImageUsageFlags flags;
        if (usage & Graphics::GPUTextureUsage::RenderTarget) flags |= vk::ImageUsageFlagBits::eColorAttachment;
        if (usage & Graphics::GPUTextureUsage::DepthStencil) flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
        if (usage & Graphics::GPUTextureUsage::ShaderUsage) flags |= vk::ImageUsageFlagBits::eSampled;
        if (usage & Graphics::GPUTextureUsage::Storage) flags |= vk::ImageUsageFlagBits::eStorage;
        if (usage & Graphics::GPUTextureUsage::TransferSrc) flags |= vk::ImageUsageFlagBits::eTransferSrc;
        if (usage & Graphics::GPUTextureUsage::TransferDst) flags |= vk::ImageUsageFlagBits::eTransferDst;
        return flags;
    }
}