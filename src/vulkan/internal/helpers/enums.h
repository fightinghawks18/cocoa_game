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

    inline vk::Format GPUFormatToVk(Graphics::GPUFormat format) {
        switch (format) {
            case Graphics::GPUFormat::Unknown:
                return vk::Format::eUndefined;
            case Graphics::GPUFormat::BGRA8_SRGB:
                return vk::Format::eB8G8R8A8Srgb;
            case Graphics::GPUFormat::RGB32_SFLOAT:
                return vk::Format::eR32G32B32Sfloat;
            case Graphics::GPUFormat::RGBA32_SFLOAT:
                return vk::Format::eR32G32B32A32Sfloat;
        }
    }

    inline vk::PrimitiveTopology GPUTopologyToVk(Graphics::GPUTopology topology) {
        switch (topology) {
            case Graphics::GPUTopology::TriangleList:
                return vk::PrimitiveTopology::eTriangleList;
        }
    }

    inline vk::PolygonMode GPUPolygonModeToVk(Graphics::GPUPolygonMode polygonMode) {
        switch (polygonMode) {
            case Graphics::GPUPolygonMode::Fill:
                return vk::PolygonMode::eFill;
            case Graphics::GPUPolygonMode::Line:
                return vk::PolygonMode::eLine;
        }
    }

    inline vk::CullModeFlags GPUCullModeToVk(Graphics::GPUCullMode cullMode) {
        switch (cullMode) {
            case Graphics::GPUCullMode::None:
                return vk::CullModeFlagBits::eNone;
            case Graphics::GPUCullMode::Frontside:
                return vk::CullModeFlagBits::eFront;
            case Graphics::GPUCullMode::Backside:
                return vk::CullModeFlagBits::eBack;
            case Graphics::GPUCullMode::Both:
                return vk::CullModeFlagBits::eFrontAndBack;
        }
    }

    inline vk::FrontFace GPUFrontFaceToVk(Graphics::GPUFrontFace frontFace) {
        switch (frontFace) {
            case Graphics::GPUFrontFace::Clockwise:
                return vk::FrontFace::eClockwise;
            case Graphics::GPUFrontFace::CounterClockwise:
                return vk::FrontFace::eCounterClockwise;
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