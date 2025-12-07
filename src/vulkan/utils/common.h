//
// Created by fightinghawks18 on 12/7/2025.
//

#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Weverything"
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

#include <vulkan/vulkan.hpp>

#include "../../graphics/utils/enums.h"
#include "../../graphics/utils/flags.h"

#include <variant>

namespace Cocoa::Vulkan {
    struct LayoutTransitionInfo
    {
        vk::PipelineStageFlags2 stage;
        vk::AccessFlags2 access;
    };

    inline LayoutTransitionInfo GetLayoutInfo(const vk::ImageLayout layout)
    {
        switch (layout) {
        case vk::ImageLayout::eUndefined: return {vk::PipelineStageFlagBits2::eNone, vk::AccessFlagBits2::eNone};
        case vk::ImageLayout::eGeneral:
            return {
                vk::PipelineStageFlagBits2::eAllCommands,
                vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite
            };
        case vk::ImageLayout::eColorAttachmentOptimal:
            return {
                vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                vk::AccessFlagBits2::eColorAttachmentWrite | vk::AccessFlagBits2::eColorAttachmentRead
            };
        case vk::ImageLayout::eDepthStencilAttachmentOptimal:
            return {
                vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
                vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite
            };
        case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
            return {
                vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
                vk::AccessFlagBits2::eDepthStencilAttachmentRead
            };
        case vk::ImageLayout::eShaderReadOnlyOptimal:
            return {
                vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader |
                    vk::PipelineStageFlagBits2::eComputeShader,
                vk::AccessFlagBits2::eShaderRead
            };
        case vk::ImageLayout::eTransferSrcOptimal:
            return {vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferRead};
        case vk::ImageLayout::eTransferDstOptimal:
            return {vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eTransferWrite};
        case vk::ImageLayout::ePresentSrcKHR:
            return {vk::PipelineStageFlagBits2::eBottomOfPipe, vk::AccessFlagBits2::eNone};
        case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
        case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
            return {
                vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
                vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite
            };
        default:
            return {
                vk::PipelineStageFlagBits2::eAllCommands,
                vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite
            };
        }
    }

    inline vk::AttachmentLoadOp GPUPassLoadOpToVk(const Graphics::GPUPassLoadOp op)
    {
        switch (op) {
        case Graphics::GPUPassLoadOp::Clear:    return vk::AttachmentLoadOp::eClear;
        case Graphics::GPUPassLoadOp::Load:     return vk::AttachmentLoadOp::eLoad;
        case Graphics::GPUPassLoadOp::DontCare: return vk::AttachmentLoadOp::eDontCare;
        default:                                return vk::AttachmentLoadOp::eNone;
        }
    }

    inline vk::AttachmentStoreOp GPUPassStoreOpToVk(const Graphics::GPUPassStoreOp op)
    {
        switch (op) {
        case Graphics::GPUPassStoreOp::Store:    return vk::AttachmentStoreOp::eStore;
        case Graphics::GPUPassStoreOp::DontCare: return vk::AttachmentStoreOp::eDontCare;
        default:                                 return vk::AttachmentStoreOp::eNone;
        }
    }

    inline vk::Format GPUColorFormatToVk(const Graphics::GPUColorFormat format)
    {
        switch (format) {
        case Graphics::GPUColorFormat::Unknown:      return vk::Format::eUndefined;
        case Graphics::GPUColorFormat::BGRA8_SRGB:   return vk::Format::eB8G8R8A8Srgb;
        case Graphics::GPUColorFormat::RG32_Float:   return vk::Format::eR32G32Sfloat;
        case Graphics::GPUColorFormat::RGB32_Float:  return vk::Format::eR32G32B32Sfloat;
        case Graphics::GPUColorFormat::RGBA32_Float: return vk::Format::eR32G32B32A32Sfloat;
        case Graphics::GPUColorFormat::RGBA8_Unorm:  return vk::Format::eR8G8B8A8Unorm;
        default:                                     return vk::Format::eUndefined;
        }
    }

    inline vk::Format GPUDepthStencilFormatToVk(const Graphics::GPUDepthStencilFormat format)
    {
        switch (format) {
        case Graphics::GPUDepthStencilFormat::Unknown:                   return vk::Format::eUndefined;
        case Graphics::GPUDepthStencilFormat::DepthFloat32_NoStencil:    return vk::Format::eD32Sfloat;
        case Graphics::GPUDepthStencilFormat::DepthUnorm16_NoStencil:    return vk::Format::eD16Unorm;
        case Graphics::GPUDepthStencilFormat::DepthFloat32_StencilUint8: return vk::Format::eD32SfloatS8Uint;
        case Graphics::GPUDepthStencilFormat::DepthUnorm24_StencilUint8: return vk::Format::eD24UnormS8Uint;
        case Graphics::GPUDepthStencilFormat::DepthUnorm16_StencilUint8: return vk::Format::eD16UnormS8Uint;
        default:                                                         return vk::Format::eUndefined;
        }
    }

    inline vk::Filter GPUFilterToVk(const Graphics::GPUFilter filter)
    {
        switch (filter) {
        case Graphics::GPUFilter::Linear:  return vk::Filter::eLinear;
        case Graphics::GPUFilter::Nearest: return vk::Filter::eNearest;
        default:                           return vk::Filter::eLinear;
        }
    }

    inline vk::ImageType GPUTextureDimensionToVk(const Graphics::GPUTextureDimension dimension)
    {
        switch (dimension) {
        case Graphics::GPUTextureDimension::One:   return vk::ImageType::e1D;
        case Graphics::GPUTextureDimension::Two:   return vk::ImageType::e2D;
        case Graphics::GPUTextureDimension::Three: return vk::ImageType::e3D;
        default:                                   return vk::ImageType::e2D;
        }
    }

    inline vk::SamplerMipmapMode GPUMipMapModeToVk(const Graphics::GPUMipmapMode mipmapMode)
    {
        switch (mipmapMode) {
        case Graphics::GPUMipmapMode::Linear:  return vk::SamplerMipmapMode::eLinear;
        case Graphics::GPUMipmapMode::Nearest: return vk::SamplerMipmapMode::eNearest;
        default:                               return vk::SamplerMipmapMode::eLinear;
        }
    }

    inline vk::SamplerAddressMode GPUWrappingModeToVk(const Graphics::GPUWrappingMode wrappingMode)
    {
        switch (wrappingMode) {
        case Graphics::GPUWrappingMode::Repeat:         return vk::SamplerAddressMode::eRepeat;
        case Graphics::GPUWrappingMode::ClampToEdge:    return vk::SamplerAddressMode::eClampToEdge;
        case Graphics::GPUWrappingMode::ClampToBorder:  return vk::SamplerAddressMode::eClampToBorder;
        case Graphics::GPUWrappingMode::MirroredRepeat: return vk::SamplerAddressMode::eMirroredRepeat;
        default:                                        return vk::SamplerAddressMode::eRepeat;
        }
    }

    inline vk::ImageLayout GPUTextureLayoutToVk(const Graphics::GPUTextureState textureLayout)
    {
        switch (textureLayout) {
        case Graphics::GPUTextureState::Unknown:                return vk::ImageLayout::eUndefined;
        case Graphics::GPUTextureState::General:                return vk::ImageLayout::eGeneral;
        case Graphics::GPUTextureState::ColorAttachment:        return vk::ImageLayout::eColorAttachmentOptimal;
        case Graphics::GPUTextureState::DepthStencilAttachment: return vk::ImageLayout::eDepthStencilAttachmentOptimal;
        case Graphics::GPUTextureState::DepthStencilReadOnly:
            return vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
        case Graphics::GPUTextureState::ShaderReadOnly: return vk::ImageLayout::eShaderReadOnlyOptimal;
        case Graphics::GPUTextureState::Present:        return vk::ImageLayout::ePresentSrcKHR;
        case Graphics::GPUTextureState::TransferDst:    return vk::ImageLayout::eTransferDstOptimal;
        case Graphics::GPUTextureState::TransferSrc:    return vk::ImageLayout::eTransferSrcOptimal;
        default:                                         return vk::ImageLayout::eUndefined;
        }
    }

    inline Graphics::GPUColorFormat VkToGPUFormat(const vk::Format format)
    {
        switch (format) {
        case vk::Format::eUndefined:          return Graphics::GPUColorFormat::Unknown;
        case vk::Format::eB8G8R8A8Srgb:       return Graphics::GPUColorFormat::BGRA8_SRGB;
        case vk::Format::eR32G32B32Sfloat:    return Graphics::GPUColorFormat::RGB32_Float;
        case vk::Format::eR32G32B32A32Sfloat: return Graphics::GPUColorFormat::RGBA32_Float;
        case vk::Format::eR8G8B8A8Unorm:      return Graphics::GPUColorFormat::RGBA8_Unorm;
        default:                              return Graphics::GPUColorFormat::Unknown;
        }
    }

    inline vk::PrimitiveTopology GPUTopologyToVk(const Graphics::GPUTopology topology)
    {
        switch (topology) {
        case Graphics::GPUTopology::TriangleList: return vk::PrimitiveTopology::eTriangleList;
        default:                                  return vk::PrimitiveTopology::eTriangleStrip;
        }
    }

    inline vk::PolygonMode GPUPolygonModeToVk(const Graphics::GPUPolygonMode polygonMode)
    {
        switch (polygonMode) {
        case Graphics::GPUPolygonMode::Fill: return vk::PolygonMode::eFill;
        case Graphics::GPUPolygonMode::Line: return vk::PolygonMode::eLine;
        default:                             return vk::PolygonMode::eFill;
        }
    }

    inline vk::ImageViewType GPUTextureViewTypeToVk(const Graphics::GPUTextureViewType type)
    {
        switch (type) {
        case Graphics::GPUTextureViewType::OneDimensional:   return vk::ImageViewType::e1D;
        case Graphics::GPUTextureViewType::TwoDimensional:   return vk::ImageViewType::e2D;
        case Graphics::GPUTextureViewType::ThreeDimensional: return vk::ImageViewType::e3D;
        case Graphics::GPUTextureViewType::Cube:             return vk::ImageViewType::eCube;
        default:                                             return vk::ImageViewType::e2D;
        }
    }

    inline vk::CullModeFlags GPUCullModeToVk(const Graphics::GPUCullMode cullMode)
    {
        switch (cullMode) {
        case Graphics::GPUCullMode::None:      return vk::CullModeFlagBits::eNone;
        case Graphics::GPUCullMode::Frontside: return vk::CullModeFlagBits::eFront;
        case Graphics::GPUCullMode::Backside:  return vk::CullModeFlagBits::eBack;
        case Graphics::GPUCullMode::Both:      return vk::CullModeFlagBits::eFrontAndBack;
        default:                               return vk::CullModeFlagBits::eNone;
        }
    }

    inline vk::FrontFace GPUFrontFaceToVk(const Graphics::GPUFrontFace frontFace)
    {
        switch (frontFace) {
        case Graphics::GPUFrontFace::Clockwise:        return vk::FrontFace::eClockwise;
        case Graphics::GPUFrontFace::CounterClockwise:
        default:                                       return vk::FrontFace::eCounterClockwise;
        }
    }

    inline vk::DescriptorType BindGroupTypeToVk(const Graphics::GPUBindGroupType type)
    {
        switch (type) {
        case Graphics::GPUBindGroupType::UniformBuffer: return vk::DescriptorType::eUniformBuffer;
        case Graphics::GPUBindGroupType::StorageBuffer: return vk::DescriptorType::eStorageBuffer;
        case Graphics::GPUBindGroupType::Texture:       return vk::DescriptorType::eSampledImage;
        case Graphics::GPUBindGroupType::Sampler:       return vk::DescriptorType::eSampler;
        default:                                        return vk::DescriptorType::eUniformBuffer;
        }
    }

    inline VmaAllocationCreateFlags GPUMemoryAccessToVma(const Graphics::GPUMemoryAccess access)
    {
        switch (access) {
        case Graphics::GPUMemoryAccess::GPUOnly: return 0;
        case Graphics::GPUMemoryAccess::CPUToGPU:
            return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        case Graphics::GPUMemoryAccess::GPUToCPU:
            return VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        case Graphics::GPUMemoryAccess::CPUOnly:
            return VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                   VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        default: return 0;
        }
    }

    inline vk::CompareOp GPUCompareOpToVk(const Graphics::GPUCompareOp op)
    {
        switch (op) {
        case Graphics::GPUCompareOp::Never:          return vk::CompareOp::eNever;
        case Graphics::GPUCompareOp::Less:           return vk::CompareOp::eLess;
        case Graphics::GPUCompareOp::Equal:          return vk::CompareOp::eEqual;
        case Graphics::GPUCompareOp::LessOrEqual:    return vk::CompareOp::eLessOrEqual;
        case Graphics::GPUCompareOp::Greater:        return vk::CompareOp::eGreater;
        case Graphics::GPUCompareOp::NotEqual:       return vk::CompareOp::eNotEqual;
        case Graphics::GPUCompareOp::GreaterOrEqual: return vk::CompareOp::eGreaterOrEqual;
        case Graphics::GPUCompareOp::Always:         return vk::CompareOp::eAlways;
        default:                                     return vk::CompareOp::eNever;
        }
    }

    inline vk::StencilOp GPUStencilOpToVk(const Graphics::GPUStencilOp op)
    {
        switch (op) {
        case Graphics::GPUStencilOp::Keep:              return vk::StencilOp::eKeep;
        case Graphics::GPUStencilOp::Zero:              return vk::StencilOp::eZero;
        case Graphics::GPUStencilOp::Replace:           return vk::StencilOp::eReplace;
        case Graphics::GPUStencilOp::IncrementAndClamp: return vk::StencilOp::eIncrementAndClamp;
        case Graphics::GPUStencilOp::DecrementAndClamp: return vk::StencilOp::eDecrementAndClamp;
        case Graphics::GPUStencilOp::Invert:            return vk::StencilOp::eInvert;
        case Graphics::GPUStencilOp::IncrementAndWrap:  return vk::StencilOp::eIncrementAndWrap;
        case Graphics::GPUStencilOp::DecrementAndWrap:  return vk::StencilOp::eDecrementAndWrap;
        default:                                        return vk::StencilOp::eKeep;
        }
    }

    inline bool IsDepthFormat(const Graphics::GPUDepthStencilFormat depthStencilFormat)
    {
        switch (depthStencilFormat) {
        case Graphics::GPUDepthStencilFormat::DepthFloat32_NoStencil:
        case Graphics::GPUDepthStencilFormat::DepthUnorm16_NoStencil: return true;
        default:                                                      return false;
        }
    }

    inline bool IsDepthStencilFormat(const Graphics::GPUDepthStencilFormat depthStencilFormat)
    {
        switch (depthStencilFormat) {
        case Graphics::GPUDepthStencilFormat::DepthFloat32_StencilUint8:
        case Graphics::GPUDepthStencilFormat::DepthUnorm16_StencilUint8: return true;
        default:                                                         return false;
        }
    }

    inline vk::ImageAspectFlags InferAspectMasks(
        const std::variant<Graphics::GPUColorFormat, Graphics::GPUDepthStencilFormat>& format
    )
    {
        if (std::holds_alternative<Graphics::GPUDepthStencilFormat>(format)) {
            const auto depthStencilFormat = std::get<Graphics::GPUDepthStencilFormat>(format);
            if (IsDepthStencilFormat(depthStencilFormat)) {
                return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
            }
            if (IsDepthFormat(depthStencilFormat)) return vk::ImageAspectFlagBits::eDepth;
        }
        return vk::ImageAspectFlagBits::eColor;
    }

    inline vk::BufferUsageFlags GPUBufferUsageToVk(const Graphics::GPUBufferUsage stage)
    {
        vk::BufferUsageFlags flags;
        if (stage & Graphics::GPUBufferUsage::Vertex)
            flags |= vk::BufferUsageFlagBits::eVertexBuffer;
        if (stage & Graphics::GPUBufferUsage::Index)
            flags |= vk::BufferUsageFlagBits::eIndexBuffer;
        if (stage & Graphics::GPUBufferUsage::Storage)
            flags |= vk::BufferUsageFlagBits::eStorageBuffer;
        if (stage & Graphics::GPUBufferUsage::Uniform)
            flags |= vk::BufferUsageFlagBits::eUniformBuffer;
        if (stage & Graphics::GPUBufferUsage::TransferSrc)
            flags |= vk::BufferUsageFlagBits::eTransferSrc;
        if (stage & Graphics::GPUBufferUsage::TransferDst)
            flags |= vk::BufferUsageFlagBits::eTransferDst;
        return flags;
    }

    inline vk::ShaderStageFlags GPUShaderStageToVk(const Graphics::GPUShaderStage stage)
    {
        vk::ShaderStageFlags flags;
        if (stage & Graphics::GPUShaderStage::Vertex)
            flags |= vk::ShaderStageFlagBits::eVertex;
        if (stage & Graphics::GPUShaderStage::Pixel)
            flags |= vk::ShaderStageFlagBits::eFragment;
        return flags;
    }

    inline vk::ShaderStageFlagBits GPUShaderStageToVkBit(const Graphics::GPUShaderStage stage)
    {
        if (stage & Graphics::GPUShaderStage::Vertex)
            return vk::ShaderStageFlagBits::eVertex;
        if (stage & Graphics::GPUShaderStage::Pixel)
            return vk::ShaderStageFlagBits::eFragment;
        return vk::ShaderStageFlagBits::eAll;
    }

    inline vk::ImageAspectFlags GPUTextureAspectToVk(const Graphics::GPUTextureAspect aspect)
    {
        vk::ImageAspectFlags flags;
        if (aspect & Graphics::GPUTextureAspect::None)
            flags |= vk::ImageAspectFlagBits::eNone;
        if (aspect & Graphics::GPUTextureAspect::Color)
            flags |= vk::ImageAspectFlagBits::eColor;
        if (aspect & Graphics::GPUTextureAspect::Depth)
            flags |= vk::ImageAspectFlagBits::eDepth;
        if (aspect & Graphics::GPUTextureAspect::Stencil)
            flags |= vk::ImageAspectFlagBits::eStencil;
        return flags;
    }

    inline vk::ImageUsageFlags GPUTextureUsageToVk(const Graphics::GPUTextureUsage usage)
    {
        vk::ImageUsageFlags flags;
        if (usage & Graphics::GPUTextureUsage::RenderTarget)
            flags |= vk::ImageUsageFlagBits::eColorAttachment;
        if (usage & Graphics::GPUTextureUsage::DepthStencil)
            flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
        if (usage & Graphics::GPUTextureUsage::ShaderUsage)
            flags |= vk::ImageUsageFlagBits::eSampled;
        if (usage & Graphics::GPUTextureUsage::Storage)
            flags |= vk::ImageUsageFlagBits::eStorage;
        if (usage & Graphics::GPUTextureUsage::TransferSrc)
            flags |= vk::ImageUsageFlagBits::eTransferSrc;
        if (usage & Graphics::GPUTextureUsage::TransferDst)
            flags |= vk::ImageUsageFlagBits::eTransferDst;
        return flags;
    }
} // namespace Cocoa::Vulkan
