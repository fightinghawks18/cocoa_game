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

    inline vk::Filter GPUFilterToVk(Graphics::GPUFilter filter) {
        switch (filter) {
            case Graphics::GPUFilter::Linear:
                return vk::Filter::eLinear;
            case Graphics::GPUFilter::Nearest:
                return vk::Filter::eNearest;
        }
    }

    inline vk::SamplerMipmapMode GPUMipMapModeToVk(Graphics::GPUMipmapMode mipmapMode) {
        switch (mipmapMode) {
            case Graphics::GPUMipmapMode::Linear:
                return vk::SamplerMipmapMode::eLinear;
            case Graphics::GPUMipmapMode::Nearest:
                return vk::SamplerMipmapMode::eNearest;
        }
    }

    inline vk::SamplerAddressMode GPUWrappingModeToVk(Graphics::GPUWrappingMode wrappingMode) {
        switch (wrappingMode) {
            case Graphics::GPUWrappingMode::Repeat:
                return vk::SamplerAddressMode::eRepeat;
            case Graphics::GPUWrappingMode::ClampToEdge:
                return vk::SamplerAddressMode::eClampToEdge;
            case Graphics::GPUWrappingMode::ClampToBorder:
                return vk::SamplerAddressMode::eClampToBorder;
            case Graphics::GPUWrappingMode::MirroredRepeat:
                return vk::SamplerAddressMode::eMirroredRepeat;
        }
    }

    inline vk::ImageLayout GPUTextureLayoutToVk(Graphics::GPUTextureLayout textureLayout) {
        switch (textureLayout) {
            case Graphics::GPUTextureLayout::Unknown:
                return vk::ImageLayout::eUndefined;
            case Graphics::GPUTextureLayout::General:
                return vk::ImageLayout::eGeneral;
            case Graphics::GPUTextureLayout::ColorAttachment:
                return vk::ImageLayout::eColorAttachmentOptimal;
            case Graphics::GPUTextureLayout::DepthStencilAttachment:
                return vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal;
            case Graphics::GPUTextureLayout::DepthStencilReadOnly:
                return vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
            case Graphics::GPUTextureLayout::ShaderReadOnly:
                return vk::ImageLayout::eShaderReadOnlyOptimal;
            case Graphics::GPUTextureLayout::Present:
                return vk::ImageLayout::ePresentSrcKHR;
            case Graphics::GPUTextureLayout::TransferDst:
                return vk::ImageLayout::eTransferDstOptimal;
            case Graphics::GPUTextureLayout::TransferSrc:
                return vk::ImageLayout::eTransferSrcOptimal;
        }
    }

    inline Graphics::GPUFormat VkToGPUFormat(vk::Format format) {
        switch (format) {
            case vk::Format::eUndefined:
                return Graphics::GPUFormat::Unknown;
            case vk::Format::eB8G8R8A8Srgb:
                return Graphics::GPUFormat::BGRA8_SRGB;
            case vk::Format::eR32G32B32Sfloat:
                return Graphics::GPUFormat::RGB32_SFLOAT;
            case vk::Format::eR32G32B32A32Sfloat:
                return Graphics::GPUFormat::RGBA32_SFLOAT;
            default: return Graphics::GPUFormat::Unknown;
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

    inline vk::DescriptorType BindGroupTypeToVk(Graphics::GPUBindGroupType type) {
        switch (type) {
            case Graphics::GPUBindGroupType::UniformBuffer:
                return vk::DescriptorType::eUniformBuffer;
            case Graphics::GPUBindGroupType::StorageBuffer:
                return vk::DescriptorType::eStorageBuffer;
            case Graphics::GPUBindGroupType::Texture:
                return vk::DescriptorType::eUniformTexelBuffer;
            case Graphics::GPUBindGroupType::Sampler:
                return vk::DescriptorType::eSampler;
        }
    }
}