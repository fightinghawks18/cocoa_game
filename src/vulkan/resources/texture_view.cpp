#include "texture_view.h"
#include "../device.h"
#include "texture.h"

#include "../internal/helpers/enums.h"
#include "../internal/helpers/flags.h"

namespace Cocoa::Vulkan {
    TextureView::TextureView(Device* device, Texture* texture, Graphics::GPUTextureViewDesc viewDesc) : _device(device), _aspect(viewDesc.aspect) {
        vk::ComponentMapping components{};
        components.setR(vk::ComponentSwizzle::eIdentity)
                    .setG(vk::ComponentSwizzle::eIdentity)
                    .setB(vk::ComponentSwizzle::eIdentity)
                    .setA(vk::ComponentSwizzle::eIdentity);
        vk::ImageSubresourceRange subresourceRange{};
        subresourceRange.setAspectMask(GPUTextureAspectToVk(viewDesc.aspect))
                    .setBaseArrayLayer(viewDesc.firstLayer)
                    .setLayerCount(viewDesc.layers)
                    .setBaseMipLevel(viewDesc.firstLevel)
                    .setLevelCount(viewDesc.levels);
        vk::ImageViewCreateInfo imageViewDescriptor{};
        imageViewDescriptor.setImage(texture->Get())
                    .setViewType(GPUTextureViewTypeToVk(viewDesc.type))
                    .setComponents(components)
                    .setSubresourceRange(subresourceRange);
        if (std::holds_alternative<Graphics::GPUColorFormat>(viewDesc.format)) {
            imageViewDescriptor.setFormat(GPUColorFormatToVk(std::get<Graphics::GPUColorFormat>(viewDesc.format)));
        } else {
            imageViewDescriptor.setFormat(GPUDepthStencilFormatToVk(std::get<Graphics::GPUDepthStencilFormat>(viewDesc.format)));
        }
        _view = _device->GetDevice().createImageViewUnique(imageViewDescriptor);
    }
}