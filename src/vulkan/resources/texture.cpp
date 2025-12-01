#include "texture.h"
#include "../device.h"

#include "../../macros.h"

namespace Cocoa::Vulkan {
    Texture::Texture(Device* device, const vk::ImageCreateInfo* desc, vk::ImageViewCreateInfo* viewDesc) : _device(device) {
        if (desc) {
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
        }

        if (!viewDesc) return;
        viewDesc->setImage(_image);
        _imageView = _device->GetDevice().createImageView(*viewDesc);
        _allocatedView = true;
    }

    Texture::Texture(Device* device, const vk::Image image, const vk::ImageView view) : _device(device), _imageView(view), _image(image) {}

    Texture::~Texture() {
        if (!_allocatedImage && !_allocatedView) return;
        _device->GetDevice().waitIdle();

        if (!_imageView) return;
        _device->GetDevice().destroyImageView(_imageView);
        if (!_allocatedImage || !_image) return;
        vmaDestroyImage(_device->GetAllocator(), _image, _allocation);
    }
}