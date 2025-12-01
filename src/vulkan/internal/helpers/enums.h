#pragma once

#include <vulkan/vulkan.hpp>

#include "../../vma.h"
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
            case Graphics::GPUFormat::BGRA8Srgb:
                return vk::Format::eB8G8R8A8Srgb;
            case Graphics::GPUFormat::RG32Sfloat:
                return vk::Format::eR32G32Sfloat;
            case Graphics::GPUFormat::RGB32Sfloat:
                return vk::Format::eR32G32B32Sfloat;
            case Graphics::GPUFormat::RGBA32Sfloat:
                return vk::Format::eR32G32B32A32Sfloat;
            case Graphics::GPUFormat::RGBA8Unorm:
                return vk::Format::eR8G8B8A8Unorm;
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

    inline vk::ImageType GPUTextureDimensionToVk(Graphics::GPUTextureDimension dimension) {
        switch (dimension) {
            case Graphics::GPUTextureDimension::One:
                return vk::ImageType::e1D;
            case Graphics::GPUTextureDimension::Two:
                return vk::ImageType::e2D;
            case Graphics::GPUTextureDimension::Three:
                return vk::ImageType::e3D;
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
                return Graphics::GPUFormat::BGRA8Srgb;
            case vk::Format::eR32G32B32Sfloat:
                return Graphics::GPUFormat::RGB32Sfloat;
            case vk::Format::eR32G32B32A32Sfloat:
                return Graphics::GPUFormat::RGBA32Sfloat;
            case vk::Format::eR8G8B8A8Unorm:
                return Graphics::GPUFormat::RGBA8Unorm;
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
                return vk::DescriptorType::eSampledImage;
            case Graphics::GPUBindGroupType::Sampler:
                return vk::DescriptorType::eSampler;
        }
    }

    inline VmaAllocationCreateFlags GPUBufferAccessToVma(Graphics::GPUBufferAccess access) {
        switch(access) {
            case Graphics::GPUBufferAccess::GPUOnly:
                return 0;
            case Graphics::GPUBufferAccess::CPUToGPU:
                return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                       VMA_ALLOCATION_CREATE_MAPPED_BIT;
            case Graphics::GPUBufferAccess::GPUToCPU:
                return VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                       VMA_ALLOCATION_CREATE_MAPPED_BIT;
            case Graphics::GPUBufferAccess::CPUOnly:
                return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                       VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                       VMA_ALLOCATION_CREATE_MAPPED_BIT;
        }
    }
}