#include "swapchain.h"


#include "device.h"

#include "../macros.h"
#include "../graphics/enums.h"
#include "../graphics/handles.h"
#include "internal/helpers/enums.h"

namespace Cocoa::Vulkan {
    Swapchain::Swapchain(Device* device, Graphics::GFXWindowDesc desc) : _device(device), _surface(device->GetSurfaceInstance(desc.surface)) {
        CreateSwapchain();
        CreateSemaphores();
        CreateFences();
    }

    Swapchain::~Swapchain() {
        _device->GetDevice().waitIdle();

        _renderSemaphores.clear();
        _imageSemaphores.clear();
        _fences.clear();

        DestroySwapchainImages();
        _swapchain.reset();
    }

    void Swapchain::Submit(vk::CommandBuffer commandBuffer) {
        vk::SemaphoreSubmitInfo imageSubmitDescriptor{};
        imageSubmitDescriptor.setSemaphore(_imageSemaphores[_frame].get())
                    .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);
        
        vk::SemaphoreSubmitInfo renderSubmitDescriptor{};
        renderSubmitDescriptor.setSemaphore(_renderSemaphores[_frame].get())
                    .setStageMask(vk::PipelineStageFlagBits2::eAllCommands);

        vk::CommandBufferSubmitInfo commandSubmitDescriptor{};
        commandSubmitDescriptor.setCommandBuffer(commandBuffer);

        vk::SubmitInfo2 submitDescriptor{};
        submitDescriptor.setWaitSemaphoreInfos(imageSubmitDescriptor)
                    .setSignalSemaphoreInfos(renderSubmitDescriptor)
                    .setCommandBufferInfos(commandSubmitDescriptor);
        
        _device->GetQueue(Graphics::GPUQueueType::Graphics)->queue.submit2(submitDescriptor, _fences[_frame].get());
    }

    void Swapchain::Present() {
        vk::PresentInfoKHR presentDescriptor{};
        presentDescriptor.setImageIndices({_imageIndex})
                    .setSwapchains({_swapchain.get()})
                    .setWaitSemaphores({_renderSemaphores[_frame].get()});
        vk::Result presentResult = _device->GetQueue(Graphics::GPUQueueType::Graphics)->queue.presentKHR(presentDescriptor);
        if (presentResult == vk::Result::eSuboptimalKHR || presentResult == vk::Result::eErrorOutOfDateKHR) {
            Resize();
            return;
        } else if (presentResult != vk::Result::eSuccess) {
            PANIC("Failed to present!");
        }

        _frame = (_frame + 1) % 2;
    }

    Graphics::GFXTextureHandle Swapchain::GetNextBackBuffer() {
        vk::Result waitForPreviousOperations = _device->GetDevice().waitForFences(1, &_fences[_frame].get(), true, UINT64_MAX);
        if (waitForPreviousOperations != vk::Result::eSuccess) {
            PANIC("Failed to wait for previous operations");
        }
        _device->GetDevice().resetFences(_fences[_frame].get());

        vk::Result getNextSwapchainImageIndex = _device->GetDevice().acquireNextImageKHR(_swapchain.get(), UINT64_MAX, _imageSemaphores[_frame].get(), nullptr, &_imageIndex);
        if (getNextSwapchainImageIndex == vk::Result::eSuboptimalKHR || getNextSwapchainImageIndex == vk::Result::eErrorOutOfDateKHR) {
            Resize();
            return GetNextBackBuffer();
        } else if (getNextSwapchainImageIndex != vk::Result::eSuccess) {
            PANIC("Failed to get next swapchain image index!");
        }
        
        return _swapchainImages[_imageIndex];
    }

    Graphics::GFXTextureHandle Swapchain::GetCurrentBackBuffer() {
        return _swapchainImages[_imageIndex];
    }

    void Swapchain::CreateSwapchain() {
        auto surfaceFormats = _device->GetGPU().getSurfaceFormatsKHR(_surface->Get());
    
        vk::SurfaceFormatKHR chosenFormat = surfaceFormats[0];
        for (const auto& format : surfaceFormats) {
            if (format.format == vk::Format::eB8G8R8A8Srgb && 
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                chosenFormat = format;
                break;
            }
        }

        auto capabilities = _device->GetGPU().getSurfaceCapabilitiesKHR(_surface->Get());
        vk::Extent2D extent = {
            capabilities.currentExtent.width, 
            capabilities.currentExtent.height
        };

        vk::SwapchainCreateInfoKHR swapchainDescriptor{};
        swapchainDescriptor.setClipped(true)
                        .setImageExtent(extent)
                        .setImageFormat(chosenFormat.format)
                        .setImageColorSpace(chosenFormat.colorSpace)
                        .setImageSharingMode(vk::SharingMode::eExclusive)
                        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                        .setMinImageCount(2)
                        .setImageArrayLayers(1)
                        .setPresentMode(vk::PresentModeKHR::eFifo)
                        .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
                        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                        .setSurface(_surface->Get());

        auto oldSwapchain = std::move(_swapchain);
        if (oldSwapchain) {
            swapchainDescriptor.setOldSwapchain(oldSwapchain.get());
        }

        _swapchain = _device->GetDevice().createSwapchainKHRUnique(swapchainDescriptor);
        _swapchainFormat = chosenFormat.format;
        _swapchainExtent = {
            .w = extent.width,
            .h = extent.height
        };

        oldSwapchain.reset();

        auto vkSwapchainImages = _device->GetDevice().getSwapchainImagesKHR(_swapchain.get());
        for (auto& image : vkSwapchainImages) {
            Graphics::GPUTextureDesc textureDescriptor = {
                .external = image
            };

            Graphics::GPUTextureViewDesc viewDescriptor = {
                .format = VkToGPUFormat(chosenFormat.format)
            };

            auto texture = _device->CreateTexture(textureDescriptor);
            auto textureInstance = _device->GetTextureInstance(texture);
            textureInstance->CreateView(viewDescriptor);
            _swapchainImages.push_back(texture);
        }
    }

    void Swapchain::CreateFences() {
        for (uint32_t i = 0; i < 2; i++) {
            vk::FenceCreateInfo fenceDescriptor{};
            fenceDescriptor.setFlags(vk::FenceCreateFlagBits::eSignaled);
            _fences.push_back(_device->GetDevice().createFenceUnique(fenceDescriptor));
        }
    }
    
    void Swapchain::CreateSemaphores() {
        for (uint32_t i = 0; i < 2; i++) {
            vk::SemaphoreCreateInfo semaphoreDescriptor{};
            _imageSemaphores.push_back(_device->GetDevice().createSemaphoreUnique(semaphoreDescriptor));
            _renderSemaphores.push_back(_device->GetDevice().createSemaphoreUnique(semaphoreDescriptor));
        }
    }

    void Swapchain::Resize() {
        _device->GetDevice().waitIdle();

        _renderSemaphores.clear();
        _imageSemaphores.clear();
        _fences.clear();
        DestroySwapchainImages();

        CreateSwapchain();
        CreateSemaphores();
        CreateFences();

        PUSH_INFO("Resized swapchain!");
    }

    void Swapchain::DestroySwapchainImages() {
        for (const auto& textureHandle : _swapchainImages) {
            _device->DestroyTexture(textureHandle);
        }
        _swapchainImages.clear();
    }
}