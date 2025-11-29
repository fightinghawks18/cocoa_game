#pragma once

#include <vulkan/vulkan.hpp>
#include "../vma.h"

namespace Cocoa::Vulkan {
    class Device;
    class Texture {
    public:
        Texture(Device* device, vk::ImageCreateInfo desc, vk::ImageViewCreateInfo* viewDesc);
        ~Texture();

        [[nodiscard]] vk::Image Get() { return _image; }
        [[nodiscard]] vk::ImageView GetView() { return _imageView.get(); }
    private:
        Device* _device;

        vk::UniqueImageView _imageView;
        vk::Image _image;
        VmaAllocation _allocation;
    };
}