#pragma once

#include <vulkan/vulkan.hpp>

namespace Cocoa::Vulkan {
    class Device;
    class Sampler {
    public:
        Sampler(Device* device, vk::SamplerCreateInfo desc);
        ~Sampler();

        [[nodiscard]] vk::Sampler Get() { return _sampler.get(); }
    private:
        Device* _device;

        vk::UniqueSampler _sampler;
    };
}