#pragma once

#include <memory>
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
        void SetLayout(Graphics::GPUTextureLayout layout) { _layout = layout; }

        [[nodiscard]] vk::Image Get() { return _image; }
        [[nodiscard]] std::vector<TextureView*> GetViews();
        [[nodiscard]] TextureView* GetView(uint32_t index) { return _imageViews[index].get(); }
        [[nodiscard]] Graphics::GPUTextureLayout GetLayout() { return _layout; }
        [[nodiscard]] uint32_t GetLevels() { return _levels; }
        [[nodiscard]] uint32_t GetLayers() { return _layers; }
        [[nodiscard]] Graphics::Extent3D GetExtent() { return _extent; }
    private:
        Device* _device;

        bool _allocatedImage = false;
        std::vector<std::unique_ptr<TextureView>> _imageViews;
        vk::Image _image;
        VmaAllocation _allocation;
        Graphics::GPUTextureLayout _layout = Graphics::GPUTextureLayout::Unknown;
        Graphics::Extent3D _extent;
        std::variant<Graphics::GPUColorFormat, Graphics::GPUDepthStencilFormat> _format;
        uint32_t _levels;
        uint32_t _layers;
    };
}