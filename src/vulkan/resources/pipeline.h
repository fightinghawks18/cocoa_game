//
// Created by fightinghawks18 on 12/7/2025.
//

#pragma once

#include "../utils/common.h"

namespace Cocoa::Vulkan {
    struct Pipeline
    {
        vk::UniquePipeline renderPipeline;
        Graphics::GFXPipelineLayoutHandle pipelineLayout;
    };
}