#include "texture.h"
#include "../device.h"

#include "../../macros.h"

namespace Cocoa::Vulkan {
    Texture::Texture(Device* device, Graphics::TextureDesc desc) : _device(device) {
        if (!desc.external) {
            VmaAllocationCreateInfo allocationDescriptor = {0};
            allocationDescriptor.usage = VMA_MEMORY_USAGE_AUTO;
            allocationDescriptor.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

            VkImage image;
            VkResult createImage = vmaCreateImage(
                device->GetAllocator(), 
                reinterpret_cast<const VkImageCreateInfo*>(&desc), 
                &allocationDescriptor, 
                &image, 
                &_allocation, 
                nullptr
            );
            if (createImage != VK_SUCCESS) {
                PANIC("Failed to create a texture!");
            }
            _image = image;
            _allocatedImage = true;
            return;
        }

        _image = reinterpret_cast<VkImage>(desc.external);
    }

    Texture::~Texture() {
        if (!_allocatedImage && _imageViews.empty()) return;
        _device->GetDevice().waitIdle();

        _imageViews.clear();
        if (!_allocatedImage) return;
        vmaDestroyImage(_device->GetAllocator(), _image, _allocation);
    }

    TextureView* Texture::CreateView(Graphics::TextureViewDesc viewDesc) {
        auto view = std::make_unique<TextureView>(_device, this, viewDesc);
        auto ptr = view.get();
        _imageViews.push_back(std::move(view));
        return ptr;
    }

    std::vector<TextureView*> Texture::GetViews() {
        std::vector<TextureView*> rawVector;
        rawVector.reserve(_imageViews.size());
        for (const auto& view : _imageViews) {
            rawVector.push_back(view.get());
        }
        return rawVector;
    }
}