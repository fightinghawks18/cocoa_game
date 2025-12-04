#pragma once

#include <vulkan/vulkan.hpp>

namespace Cocoa::Vulkan {
    struct LayoutTransitionInfo {
        vk::PipelineStageFlags2 stage;
        vk::AccessFlags2 access;
    };

    inline LayoutTransitionInfo GetLayoutInfo(vk::ImageLayout layout) {
        switch(layout) {
            case vk::ImageLayout::eUndefined:
                return {
                    vk::PipelineStageFlagBits2::eNone,
                    vk::AccessFlagBits2::eNone
                };
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
                    vk::PipelineStageFlagBits2::eVertexShader | 
                    vk::PipelineStageFlagBits2::eFragmentShader | 
                    vk::PipelineStageFlagBits2::eComputeShader,
                    vk::AccessFlagBits2::eShaderRead
                };
            case vk::ImageLayout::eTransferSrcOptimal:
                return {
                    vk::PipelineStageFlagBits2::eTransfer,
                    vk::AccessFlagBits2::eTransferRead
                };
            case vk::ImageLayout::eTransferDstOptimal:
                return {
                    vk::PipelineStageFlagBits2::eTransfer,
                    vk::AccessFlagBits2::eTransferWrite
                };
            case vk::ImageLayout::ePresentSrcKHR:
                return {
                    vk::PipelineStageFlagBits2::eBottomOfPipe,
                    vk::AccessFlagBits2::eNone
                };
            case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
                return {
                    vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
                    vk::AccessFlagBits2::eDepthStencilAttachmentRead | 
                    vk::AccessFlagBits2::eDepthStencilAttachmentWrite
                };

            case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
                return {
                    vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
                    vk::AccessFlagBits2::eDepthStencilAttachmentRead | 
                    vk::AccessFlagBits2::eDepthStencilAttachmentWrite
                };
            default:
                return {
                    vk::PipelineStageFlagBits2::eAllCommands,
                    vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite
                };
        }
    }
}