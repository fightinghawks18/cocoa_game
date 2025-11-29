#include "texture.h"
#include "../device.h"

#include "../../macros.h"

namespace Cocoa::Vulkan {
    Texture::Texture(Device* device, vk::ImageCreateInfo desc, vk::ImageViewCreateInfo* viewDesc) : _device(device) {
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
        if (viewDesc == nullptr) return;
        viewDesc->setImage(_image);
        _imageView = _device->GetDevice().createImageViewUnique(*viewDesc);
    }

    Texture::~Texture() {
        _device->GetDevice().waitIdle();
        _imageView.reset();
        vmaDestroyImage(_device->GetAllocator(), _image, _allocation);
    }
}