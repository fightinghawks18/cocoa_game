#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace Cocoa::Vulkan {
    enum class GPUPowerPreference {
        HighPerformance,
        LowPower,
    };

    enum class GPUQueueType {
        Unknown,
        Graphics,
        Compute,
        Transfer
    };

    struct GPUQueue {
        GPUQueueType type;
        uint32_t family;
        vk::Queue queue;
    };

    inline std::string GPUQueueTypeToString(GPUQueueType type) {
        switch (type) {
        case GPUQueueType::Unknown:
            return "Unknown";
        case GPUQueueType::Graphics:
            return "Graphics";
        case GPUQueueType::Compute:
            return "Compute";
        case GPUQueueType::Transfer:
            return "Transfer";
        }
    }
}