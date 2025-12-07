//
// Created by fightinghawks18 on 12/7/2025.
//

#pragma once

#include "../utils/common.h"

namespace Cocoa::Vulkan {
    struct Buffer
    {
        vk::Buffer buffer;
        void* mapped = nullptr;
        uint64_t size;
        VmaAllocation allocation;
    };
}