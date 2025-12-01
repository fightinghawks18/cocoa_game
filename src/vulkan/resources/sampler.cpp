#include "sampler.h"
#include "../device.h"

#include "../internal/helpers/enums.h"

namespace Cocoa::Vulkan {
    Sampler::Sampler(Device* device, Graphics::SamplerDesc desc) : _device(device) {
        vk::SamplerCreateInfo samplerDescriptor{};
        samplerDescriptor.setMagFilter(GPUFilterToVk(desc.magnification))
                    .setMinFilter(GPUFilterToVk(desc.minification))
                    .setAddressModeU(GPUWrappingModeToVk(desc.horizontalWrapping))
                    .setAddressModeV(GPUWrappingModeToVk(desc.verticalWrapping))
                    .setAddressModeW(GPUWrappingModeToVk(desc.depthWrapping))
                    .setMipmapMode(GPUMipMapModeToVk(desc.mipmapMode))
                    .setBorderColor(vk::BorderColor::eFloatTransparentBlack)
                    .setMaxLod(desc.maxLevel)
                    .setMipLodBias(desc.levelBias)
                    .setMinLod(desc.minLevel);
        _sampler = device->GetDevice().createSamplerUnique(samplerDescriptor);
    }

    Sampler::~Sampler() {
        _device->GetDevice().waitIdle();
        _sampler.reset();
    }
}