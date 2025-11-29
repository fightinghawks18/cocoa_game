#include "sampler.h"
#include "../device.h"

namespace Cocoa::Vulkan {
    Sampler::Sampler(Device* device, vk::SamplerCreateInfo desc) : _device(device) {
        _sampler = device->GetDevice().createSamplerUnique(desc);
    }

    Sampler::~Sampler() {
        _device->GetDevice().waitIdle();
        _sampler.reset();
    }
}