#pragma once

#include <string>
#include "enums.h"

namespace Cocoa::Graphics {
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