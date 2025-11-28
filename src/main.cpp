#include <iostream>
#include <memory>

#include <vulkan/vulkan.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "vulkan/device.h"
#include "vulkan/surface.h"
#include "vulkan/swapchain.h"

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error("Failed to start SDL3");
    }

    SDL_Window* window = SDL_CreateWindow("Cocoa", 800, 600, SDL_WINDOW_VULKAN);

    Cocoa::Vulkan::DeviceDesc desc = {
        .window = window,
        .desiredQueues = {
            Cocoa::Vulkan::GPUQueueType::Graphics,
            Cocoa::Vulkan::GPUQueueType::Transfer,
            Cocoa::Vulkan::GPUQueueType::Compute
        },
        .powerPreference = Cocoa::Vulkan::GPUPowerPreference::HighPerformance,
    };
    
    auto renderDevice = std::make_unique<Cocoa::Vulkan::Device>(desc);
    auto surface = std::make_unique<Cocoa::Vulkan::Surface>(renderDevice.get(), window);
    
    Cocoa::Vulkan::SwapchainDesc swapchainDesc = {
        .surface = surface.get()
    };
    auto swapchain = std::make_unique<Cocoa::Vulkan::Swapchain>(renderDevice.get(), swapchainDesc);

    bool gameRun = true;
    while (gameRun) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                gameRun = false;
                break;
            }
        }

        auto encoder = renderDevice->Encode(swapchain.get());

         vk::ClearColorValue clearColor;
        clearColor.setFloat32({0.6f, 0.21f, 0.36f, 1.0f});

        vk::RenderingAttachmentInfo renderingAttachmentDescriptor;
        renderingAttachmentDescriptor.setClearValue(clearColor)
                    .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
                    .setLoadOp(vk::AttachmentLoadOp::eClear)
                    .setStoreOp(vk::AttachmentStoreOp::eStore)
                    .setImageView(swapchain->GetCurrentBackBuffer().imageView);
        
        vk::RenderingInfo renderingDescriptor;
        renderingDescriptor.setColorAttachments(renderingAttachmentDescriptor)
                    .setRenderArea({{0, 0}, {800, 600}})
                    .setViewMask(0)
                    .setLayerCount(1);

        vk::Extent2D extent;
        extent.setWidth(800)
                .setHeight(600);

        vk::Rect2D scissor;
        scissor.setOffset({0, 0})
                .setExtent(extent);
        vk::Viewport viewport;
        viewport.setWidth(extent.width)
                .setHeight(extent.height)
                .setMaxDepth(1)
                .setMinDepth(0)
                .setX(0)
                .setY(0);

        encoder->StartRendering(renderingDescriptor);
        encoder->SetViewport(viewport);
        encoder->SetScissor(scissor);
        encoder->EndRendering();

        renderDevice->EndEncoding(std::move(encoder));
    }

    std::cout << "Hello World!" << std::endl;
    
    swapchain.reset();
    surface.reset();
    renderDevice.reset();
    SDL_Quit();
    return 0;
}
