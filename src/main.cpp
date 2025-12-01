#include <filesystem>
#include <iostream>
#include <memory>

#include <vulkan/vulkan.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "objects/camera.h"
#include "objects/transform.h"
#include "tools/rich_presence.h"
#include "vulkan/common.h"
#include "vulkan/device.h"
#include "vulkan/surface.h"
#include "vulkan/swapchain.h"
#include "vulkan/resources/shader_module.h"
#include "vulkan/resources/render_pipeline.h"
#include "vulkan/resources/pipeline_layout.h"
#include "vulkan/resources/buffer.h"
#include "vulkan/resources/bind_group.h"

#include "macros.h"

constexpr float sensitivity = 0.1f;

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        PANIC("Failed to start SDL3");
    }

    SDL_Window* window = SDL_CreateWindow("Cocoa", 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    Cocoa::Graphics::DeviceDesc desc = {
        .window = window,
        .desiredQueues = {
            Cocoa::Graphics::GPUQueueType::Graphics,
            Cocoa::Graphics::GPUQueueType::Transfer,
            Cocoa::Graphics::GPUQueueType::Compute
        },
        .powerPreference = Cocoa::Graphics::GPUPowerPreference::HighPerformance,
    };
    
    auto renderDevice = std::make_unique<Cocoa::Vulkan::Device>(desc);
    auto surface = renderDevice->CreateSurface(window);
    
    Cocoa::Graphics::SwapchainDesc swapchainDesc = {
        .surface = surface
    };
    auto swapchain = renderDevice->CreateSwapchain(swapchainDesc);
    auto swapchainInstance = renderDevice->GetSwapchainInstance(swapchain);

    Cocoa::Graphics::ShaderModuleDesc vertexShaderDescriptor = {
        .shaderPath = "shaders/vertex.vert.spv"
    };
    auto vertexShader = renderDevice->CreateShaderModule(vertexShaderDescriptor);

    Cocoa::Graphics::ShaderModuleDesc pixelShaderDescriptor = {
        .shaderPath = "shaders/vertex.frag.spv"
    };
    auto pixelShader = renderDevice->CreateShaderModule(pixelShaderDescriptor);
    
    Cocoa::Graphics::BufferDesc mvpBufferDescriptor = {
        .usage = Cocoa::Graphics::GPUBufferUsage::Uniform,
        .size = sizeof(Cocoa::Graphics::MVP),
        .mapped = nullptr
    };
    auto mvpBuffer = renderDevice->CreateBuffer(mvpBufferDescriptor);

    // Image sampled
    auto path = (std::filesystem::current_path() / "content" / "texture.png");
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!pixels) {
        PANIC("Failed to load image: %s %s", stbi_failure_reason(), path.string().c_str());
    }

    uint64_t imageSize = width * height * 4;

    Cocoa::Graphics::BufferDesc stagingBufferDescriptor = {
        .usage = Cocoa::Graphics::GPUBufferUsage::TransferSrc,
        .size = imageSize,
        .mapped = pixels
    };
    auto stagingBuffer = renderDevice->CreateBuffer(stagingBufferDescriptor);  

    Cocoa::Graphics::Extent3D extent;
    extent.w = width;
    extent.h = height;
    extent.d = 1;

    Cocoa::Graphics::TextureDesc imageTextureDescriptor = {
        .dimension = Cocoa::Graphics::GPUTextureDimension::Two,
        .usage = Cocoa::Graphics::GPUTextureUsage::ShaderUsage | Cocoa::Graphics::GPUTextureUsage::TransferDst,
        .format = Cocoa::Graphics::GPUFormat::RGBA8Unorm,
        .initialLayout = Cocoa::Graphics::GPUTextureLayout::Unknown,
        .samples = Cocoa::Graphics::GPUSamplingCount::None,
        extent,
        .levels = 1,
        .layers = 1,
    };

    Cocoa::Graphics::TextureViewDesc imageViewTextureDescriptor = {
        .type = Cocoa::Graphics::GPUTextureViewType::TwoDimensional,
        .format = Cocoa::Graphics::GPUFormat::RGBA8Unorm,
        .aspect = Cocoa::Graphics::GPUTextureAspect::Color,
        0, 1,
        0, 1
    };

    auto imageTexture = renderDevice->CreateTexture(imageTextureDescriptor);
    renderDevice->GetTextureInstance(imageTexture)->CreateView(imageViewTextureDescriptor);

    renderDevice->EncodeImmediateCommands([&](Cocoa::Vulkan::Encoder& encoder) {
        encoder.UploadBufferToImage(stagingBuffer, imageTexture);
        encoder.TransitionTexture(imageTexture, Cocoa::Graphics::GPUTextureLayout::ShaderReadOnly);
    });
    renderDevice->DestroyBuffer(stagingBuffer);

    Cocoa::Graphics::SamplerDesc samplerDescriptor = {};
    auto sampler = renderDevice->CreateSampler(samplerDescriptor);

    Cocoa::Graphics::BindGroupLayoutDesc meshBindGroupLayoutDescriptor{};
    meshBindGroupLayoutDescriptor.Entry(Cocoa::Graphics::GPUShaderStage::Vertex, Cocoa::Graphics::GPUBindGroupType::UniformBuffer)
                .Entry(Cocoa::Graphics::GPUShaderStage::Pixel, Cocoa::Graphics::GPUBindGroupType::Texture)
                .Entry(Cocoa::Graphics::GPUShaderStage::Pixel, Cocoa::Graphics::GPUBindGroupType::Sampler);
    auto meshBindGroupLayout = renderDevice->CreateBindGroupLayout(meshBindGroupLayoutDescriptor);

    Cocoa::Graphics::BindGroupDesc meshBindGroupDescriptor{meshBindGroupLayout};
    meshBindGroupDescriptor.Entry(mvpBuffer)
                .Entry(imageTexture)
                .Entry(sampler);
    auto meshBindGroup = renderDevice->CreateBindGroup(meshBindGroupDescriptor);

    Cocoa::Graphics::PipelineLayoutDesc pipelineLayoutDesc{};
    pipelineLayoutDesc.BindGroup(meshBindGroupLayout);

    auto pipelineLayout = renderDevice->CreatePipelineLayout(pipelineLayoutDesc);
    Cocoa::Graphics::PipelineDesc pipelineDescriptor{};
    pipelineDescriptor.Shader(Cocoa::Graphics::GPUShaderStage::Vertex, vertexShader);
    pipelineDescriptor.Shader(Cocoa::Graphics::GPUShaderStage::Pixel, pixelShader);
    pipelineDescriptor.Binding(sizeof(Cocoa::Graphics::Vertex))
                .Attribute(Cocoa::Graphics::GPUFormat::RGB32Sfloat, offsetof(Cocoa::Graphics::Vertex, pos))
                .Attribute(Cocoa::Graphics::GPUFormat::RGBA32Sfloat, offsetof(Cocoa::Graphics::Vertex, col))
                .Attribute(Cocoa::Graphics::GPUFormat::RG32Sfloat, offsetof(Cocoa::Graphics::Vertex, uv));
    pipelineDescriptor.cullMode = Cocoa::Graphics::GPUCullMode::None;
    pipelineDescriptor.pipelineLayout = pipelineLayout;

    auto renderPipeline = renderDevice->CreateRenderPipeline(pipelineDescriptor);

    auto plane = Cocoa::Vulkan::CreatePlaneIndexed();

    Cocoa::Graphics::BufferDesc vertexBufferDescriptor = {
        .usage = Cocoa::Graphics::GPUBufferUsage::Vertex,
        .size = sizeof(Cocoa::Graphics::Vertex) * plane.vertices.size(),
        .mapped = plane.vertices.data()
    };
    auto vertexBuffer = renderDevice->CreateBuffer(vertexBufferDescriptor);

    Cocoa::Graphics::BufferDesc indexBufferDescriptor = {
        .usage = Cocoa::Graphics::GPUBufferUsage::Index,
        .size = sizeof(uint16_t) * plane.indices.size(),
        .mapped = plane.indices.data()
    };
    auto indexBuffer = renderDevice->CreateBuffer(indexBufferDescriptor);  

    Cocoa::Graphics::MVP mvpData;
    
    // Objects
    Cocoa::Objects::Transform transform;

    Cocoa::Objects::Camera camera;
    camera.GetTransform().Translate(Cocoa::Math::Vector3(0, 0, 5));
    camera.SetFieldOfView(95);
    camera.SetClipFarBounds(100);
    camera.SetClipNearBounds(0.1);

    // Rich presence (for fun)
    Cocoa::Tools::RichPresenceDesc rpcDescriptor = {
        .appID = "1444737693090316409"
    };
    Cocoa::Tools::RichPresence rpc(rpcDescriptor);
    rpc.SetState("Watching a plane");
    rpc.StartTimestamp();

    // SDL3 states
    bool mouseCaptured = false;

    bool gameRun = true;
    while (gameRun) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                gameRun = false;
                break;
            }

            if (mouseCaptured && e.type == SDL_EVENT_MOUSE_MOTION) {
                camera.GetTransform().RotateY(-e.motion.xrel * sensitivity);
                camera.GetTransform().RotateX(-e.motion.yrel * sensitivity);
            }

            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE) {
                mouseCaptured = !mouseCaptured;
                SDL_SetWindowRelativeMouseMode(window, mouseCaptured);
            }
        }

        // Do some movement
        const bool* keyStates = SDL_GetKeyboardState(nullptr);
    
        Cocoa::Math::Vector3 input(0, 0, 0);
        if (keyStates[SDL_SCANCODE_W]) input.z += 1.0f;
        if (keyStates[SDL_SCANCODE_S]) input.z -= 1.0f;

        if (keyStates[SDL_SCANCODE_A]) input.x -= 1.0f;
        if (keyStates[SDL_SCANCODE_D]) input.x += 1.0f;

        if (keyStates[SDL_SCANCODE_SPACE]) input.y += 1.0f;
        if (keyStates[SDL_SCANCODE_LCTRL]) input.y -= 1.0f;
        
        if (input.LengthSquared() > 0) {
            input.Normalize();
            auto offset = camera.GetTransform().GetForward() * input.z + 
                          camera.GetTransform().GetRight() * input.x +
                          Cocoa::Math::Vector3(0, 1, 0) * input.y;
            camera.GetTransform().Translate(offset * 0.1);
        }

        // Update RPC
        rpc.Update();

        transform.RotateX(0.15);
        transform.RotateY(0.3);
        transform.RotateZ(0.35);
        
        auto backBuffer = swapchainInstance->GetNextBackBuffer();
        auto swapchainExtent = swapchainInstance->GetExtent();

        camera.SetAspectRatio(static_cast<float>(swapchainExtent.w) / static_cast<float>(swapchainExtent.h));

        mvpData.model = transform.GetModelMatrix().Transpose();
        mvpData.projection = camera.GetProjectionMatrix().Transpose();
        mvpData.view = camera.GetViewMatrix().Transpose();
        renderDevice->GetBufferInstance(mvpBuffer)->MapTo(&mvpData, sizeof(Cocoa::Graphics::MVP), 0);
        
        auto encoder = renderDevice->Encode(swapchain);

        Cocoa::Graphics::GPUColorPassDesc colorPassDesc = {
            .texture = backBuffer,
            .clearColor = {0.0f, 0.0f, 0.0f, 1.0f},
            .loadOp = Cocoa::Graphics::GPUPassLoadOp::Clear,
            .storeOp = Cocoa::Graphics::GPUPassStoreOp::Store
        };

        Cocoa::Graphics::GPUPassDesc passDesc = {
            .colorPasses = {colorPassDesc},
            .depthPass = nullptr,
            .renderArea = {
                .offset = {0},
                .extent = swapchainExtent
            },
            .viewMask = 0,
            .layerCount = 1
        };

        Cocoa::Graphics::Viewport viewport = {
            .offset = {0},
            .extent = swapchainExtent,
        };

        Cocoa::Graphics::Rect scissor = {
            .offset = viewport.offset,
            .extent = viewport.extent
        };

        encoder.StartRendering(passDesc);
        {
            encoder.SetRenderPipeline(renderPipeline);
            encoder.SetViewport(viewport);
            encoder.SetScissor(scissor);
            encoder.SetBindGroup(pipelineLayout, meshBindGroup);
            encoder.SetVertexBuffer(vertexBuffer);
            encoder.SetIndexBuffer(indexBuffer);
            encoder.DrawIndexed(plane.indices.size(), 1, 0, 0, 0);
        }
        encoder.EndRendering();
        renderDevice->EndEncoding(encoder);
    }

    std::cout << "Hello World!" << std::endl;
    renderDevice.reset();
    SDL_Quit();
    return 0;
}
