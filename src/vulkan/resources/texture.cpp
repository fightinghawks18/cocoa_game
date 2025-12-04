#include "texture.h"
#include "../device.h"

#include "../../macros.h"
#include "../internal/helpers/enums.h"
#include "../internal/helpers/flags.h"

namespace Cocoa::Vulkan {
    Texture::Texture(Device* device, Graphics::TextureDesc desc) : _device(device), _layout(desc.initialLayout), _extent(desc.scale), _levels(desc.levels), _layers(desc.layers) {
        if (!desc.external) {
            VmaAllocationCreateInfo allocationDescriptor = {0};
            allocationDescriptor.usage = VMA_MEMORY_USAGE_AUTO;
            allocationDescriptor.flags = GPUMemoryAccessToVma(desc.access);

            vk::ImageCreateInfo imageDescriptor{};
            imageDescriptor.setImageType(GPUTextureDimensionToVk(desc.dimension))
                        .setArrayLayers(desc.layers)
                        .setExtent(vk::Extent3D{desc.scale.w, desc.scale.h, desc.scale.d})
                        .setInitialLayout(GPUTextureLayoutToVk(desc.initialLayout))
                        .setMipLevels(desc.levels)
                        .setSamples(vk::SampleCountFlagBits::e1)
                        .setSharingMode(vk::SharingMode::eExclusive)
                        .setUsage(GPUTextureUsageToVk(desc.usage))
                        .setTiling(vk::ImageTiling::eOptimal);
            if (std::holds_alternative<Graphics::GPUColorFormat>(desc.format)) {
                imageDescriptor.setFormat(GPUColorFormatToVk(std::get<Graphics::GPUColorFormat>(desc.format)));
                _format = desc.format;
            } else {
                imageDescriptor.setFormat(GPUDepthStencilFormatToVk(std::get<Graphics::GPUDepthStencilFormat>(desc.format)));
                _format = desc.format;
            }

            VkImage image;
            VkResult createImage = vmaCreateImage(
                device->GetAllocator(), 
                reinterpret_cast<const VkImageCreateInfo*>(&imageDescriptor), 
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