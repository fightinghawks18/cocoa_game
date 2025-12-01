#pragma once

#include <vulkan/vulkan.hpp>
#include "../vma.h"

#include "texture_view.h"
namespace Cocoa::Vulkan {
    class Device;
    class Texture {
    public:
        Texture(Device* device, Graphics::TextureDesc desc);
        ~Texture();

        Texture(const Texture& other) = delete;
        Texture(Texture&& other) noexcept = default;
        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) noexcept = default;

        TextureView* CreateView(Graphics::TextureViewDesc viewDesc);

        [[nodiscard]] vk::Image Get() { return _image; }
        [[nodiscard]] std::vector<TextureView*> GetViews();
        [[nodiscard]] TextureView* GetView(uint32_t index) { return _imageViews[index].get(); }
    private:
        Device* _device;

        bool _allocatedImage = false;
        std::vector<std::unique_ptr<TextureView>> _imageViews;
        vk::Image _image;
        VmaAllocation _allocation;
    };
}