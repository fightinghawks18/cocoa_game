#pragma once

#include <vulkan/vulkan.hpp>
#include "../../graphics/descriptors.h"

namespace Cocoa::Vulkan {
    class Device;
    class Sampler {
    public:
        Sampler(Device* device, Graphics::SamplerDesc desc);
        ~Sampler();

        Sampler(const Sampler& other) = delete;
        Sampler(Sampler&& other) noexcept = default;
        Sampler& operator=(const Sampler& other) = delete;
        Sampler& operator=(Sampler&& other) noexcept = default;

        [[nodiscard]] vk::Sampler Get() { return _sampler.get(); }
    private:
        Device* _device;

        vk::UniqueSampler _sampler;
    };
}