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

    inline vk::Format GPUColorFormatToVk(Graphics::GPUColorFormat format) {
        switch (format) {
            case Graphics::GPUColorFormat::Unknown:
                return vk::Format::eUndefined;
            case Graphics::GPUColorFormat::BGRA8_SRGB:
                return vk::Format::eB8G8R8A8Srgb;
            case Graphics::GPUColorFormat::RG32_Float:
                return vk::Format::eR32G32Sfloat;
            case Graphics::GPUColorFormat::RGB32_Float:
                return vk::Format::eR32G32B32Sfloat;
            case Graphics::GPUColorFormat::RGBA32_Float:
                return vk::Format::eR32G32B32A32Sfloat;
            case Graphics::GPUColorFormat::RGBA8_Unorm:
                return vk::Format::eR8G8B8A8Unorm;
        }
    }

    inline vk::Format GPUDepthStencilFormatToVk(Graphics::GPUDepthStencilFormat format) {
        switch (format) {
            case Graphics::GPUDepthStencilFormat::Unknown:
                return vk::Format::eUndefined;
            case Graphics::GPUDepthStencilFormat::DepthFloat32_NoStencil:
                return vk::Format::eD32Sfloat;
            case Graphics::GPUDepthStencilFormat::DepthUnorm16_NoStencil:
                return vk::Format::eD16Unorm;
            case Graphics::GPUDepthStencilFormat::DepthFloat32_StencilUint8:
                return vk::Format::eD32SfloatS8Uint;
            case Graphics::GPUDepthStencilFormat::DepthUnorm24_StencilUint8:
                return vk::Format::eD24UnormS8Uint;
            case Graphics::GPUDepthStencilFormat::DepthUnorm16_StencilUint8:
                return vk::Format::eD16UnormS8Uint;
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

    inline Graphics::GPUColorFormat VkToGPUFormat(vk::Format format) {
        switch (format) {
            case vk::Format::eUndefined:
                return Graphics::GPUColorFormat::Unknown;
            case vk::Format::eB8G8R8A8Srgb:
                return Graphics::GPUColorFormat::BGRA8_SRGB;
            case vk::Format::eR32G32B32Sfloat:
                return Graphics::GPUColorFormat::RGB32_Float;
            case vk::Format::eR32G32B32A32Sfloat:
                return Graphics::GPUColorFormat::RGBA32_Float;
            case vk::Format::eR8G8B8A8Unorm:
                return Graphics::GPUColorFormat::RGBA8_Unorm;
            default: return Graphics::GPUColorFormat::Unknown;
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

    inline vk::ImageViewType GPUTextureViewTypeToVk(Graphics::GPUTextureViewType type) {
        switch (type) {
            case Graphics::GPUTextureViewType::OneDimensional:
                return vk::ImageViewType::e1D;
            case Graphics::GPUTextureViewType::TwoDimensional:
                return vk::ImageViewType::e2D;
            case Graphics::GPUTextureViewType::ThreeDimensional:
                return vk::ImageViewType::e3D;
            case Graphics::GPUTextureViewType::Cube:
                return vk::ImageViewType::eCube;
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

    inline VmaAllocationCreateFlags GPUMemoryAccessToVma(Graphics::GPUMemoryAccess access) {
        switch(access) {
            case Graphics::GPUMemoryAccess::GPUOnly:
                return 0;
            case Graphics::GPUMemoryAccess::CPUToGPU:
                return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                       VMA_ALLOCATION_CREATE_MAPPED_BIT;
            case Graphics::GPUMemoryAccess::GPUToCPU:
                return VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
                       VMA_ALLOCATION_CREATE_MAPPED_BIT;
            case Graphics::GPUMemoryAccess::CPUOnly:
                return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                       VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                       VMA_ALLOCATION_CREATE_MAPPED_BIT;
        }
    }

    inline vk::CompareOp GPUCompareOpToVk(Graphics::GPUCompareOp op) {
        switch (op) {
            case Graphics::GPUCompareOp::Never: 
                return vk::CompareOp::eNever;
            case Graphics::GPUCompareOp::Less:
                return vk::CompareOp::eLess;
            case Graphics::GPUCompareOp::Equal:
                return vk::CompareOp::eEqual;
            case Graphics::GPUCompareOp::LessOrEqual:
                return vk::CompareOp::eLessOrEqual;
            case Graphics::GPUCompareOp::Greater:
                return vk::CompareOp::eGreater;
            case Graphics::GPUCompareOp::NotEqual:
                return vk::CompareOp::eNotEqual;
            case Graphics::GPUCompareOp::GreaterOrEqual:
                return vk::CompareOp::eGreaterOrEqual;
            case Graphics::GPUCompareOp::Always:
                return vk::CompareOp::eAlways;
        }
    }

    inline vk::StencilOp GPUStencilOpToVk(Graphics::GPUStencilOp op) {
        switch (op) {
            case Graphics::GPUStencilOp::Keep:  
                return vk::StencilOp::eKeep;
            case Graphics::GPUStencilOp::Zero:
                return vk::StencilOp::eZero;
            case Graphics::GPUStencilOp::Replace:  
                return vk::StencilOp::eReplace;
            case Graphics::GPUStencilOp::IncrementAndClamp:
                return vk::StencilOp::eIncrementAndClamp;
            case Graphics::GPUStencilOp::DecrementAndClamp:
                return vk::StencilOp::eDecrementAndClamp;
            case Graphics::GPUStencilOp::Invert:
                return vk::StencilOp::eInvert;
            case Graphics::GPUStencilOp::IncrementAndWrap:
                return vk::StencilOp::eIncrementAndWrap;
            case Graphics::GPUStencilOp::DecrementAndWrap:
                return vk::StencilOp::eDecrementAndWrap;
        }
    }
}