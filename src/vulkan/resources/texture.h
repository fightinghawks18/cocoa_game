#pragma once

#include <vulkan/vulkan.hpp>
#include "../vma.h"

namespace Cocoa::Vulkan {
    class Device;
    class Texture {
    public:
        Texture(Device* device, const vk::ImageCreateInfo* desc, vk::ImageViewCreateInfo* viewDesc);
        Texture(Device* device, const vk::Image image, const vk::ImageView view);
        ~Texture();

        Texture(const Texture& other) = delete;
        Texture(Texture&& other) noexcept = default;
        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) noexcept = default;

        [[nodiscard]] vk::Image Get() { return _image; }
        [[nodiscard]] vk::ImageView GetView() { return _imageView; }
    private:
        Device* _device;

        bool _allocatedImage = false;
        bool _allocatedView = false;
        vk::ImageView _imageView;
        vk::Image _image;
        VmaAllocation _allocation;
    };
}