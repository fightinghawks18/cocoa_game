#pragma once

#include <vulkan/vulkan.hpp>
#include "../vma.h"

namespace Cocoa::Vulkan {
    class Device;
    class Texture {
    public:
        Texture(Device* device, vk::ImageCreateInfo desc, vk::ImageViewCreateInfo* viewDesc);
        ~Texture();

        Texture(const Texture& other) = delete;
        Texture(Texture&& other) noexcept = default;
        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) noexcept = default;

        [[nodiscard]] vk::Image Get() { return _image; }
        [[nodiscard]] vk::ImageView GetView() { return _imageView.get(); }
    private:
        Device* _device;

        vk::UniqueImageView _imageView;
        vk::Image _image;
        VmaAllocation _allocation;
    };
}