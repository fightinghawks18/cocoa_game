#include "texture_view.h"
#include "../device.h"
#include "texture.h"

#include "../internal/helpers/enums.h"

namespace Cocoa::Vulkan {
    TextureView::TextureView(Device* device, Texture* texture, Graphics::TextureViewDesc viewDesc) : _device(device), _texture(texture) {
        vk::ComponentMapping components{};
        components.setR(vk::ComponentSwizzle::eIdentity)
                    .setG(vk::ComponentSwizzle::eIdentity)
                    .setB(vk::ComponentSwizzle::eIdentity)
                    .setA(vk::ComponentSwizzle::eIdentity);
        vk::ImageSubresourceRange subresourceRange{};
        subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(viewDesc.firstLayer)
                    .setLayerCount(viewDesc.layers)
                    .setBaseMipLevel(viewDesc.firstLevel)
                    .setLevelCount(viewDesc.levels);
        vk::ImageViewCreateInfo imageViewDescriptor{};
        imageViewDescriptor.setImage(texture->Get())
                    .setViewType(vk::ImageViewType::e2D)
                    .setComponents(components)
                    .setFormat(GPUFormatToVk(viewDesc.format))
                    .setSubresourceRange(subresourceRange);
        _view = _device->GetDevice().createImageViewUnique(imageViewDescriptor);
    }
}