#pragma once

#include <vulkan/vulkan.hpp>
#include "../../graphics/descriptors.h"

namespace Cocoa::Vulkan {
    class Device;
    class Texture;
    class TextureView {
    public:
        TextureView(Device* device, Texture* texture, Graphics::TextureViewDesc viewDesc);
        ~TextureView() = default;
    
        [[nodiscard]] vk::ImageView Get() { return _view.get(); }
        [[nodiscard]] Graphics::GPUTextureAspect GetAspect() { return _aspect; }
    private:
        Device* _device;
        Texture* _texture;

        vk::UniqueImageView _view;
        Graphics::GPUTextureAspect _aspect;
    };
}