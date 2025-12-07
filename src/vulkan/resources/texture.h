//
// Created by fightinghawks18 on 12/7/2025.
//

#pragma once

#include "../../graphics/utils/types.h"
#include "../utils/common.h"

namespace Cocoa::Vulkan {
    struct Texture
    {
        VmaAllocation allocation;
        vk::Image image;
        std::variant<Graphics::GPUColorFormat, Graphics::GPUDepthStencilFormat> format;
        Graphics::GPUTextureState layout = Graphics::GPUTextureState::Unknown;
        Graphics::Scale3D extent;
        uint32_t levels;
        uint32_t layers;
        bool allocatedImage = false;
    };
}