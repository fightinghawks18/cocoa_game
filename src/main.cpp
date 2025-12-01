#include <iostream>
#include <memory>

#include <vulkan/vulkan.hpp>

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
        .shaderPath = "content/vertex.vert.spv"
    };
    auto vertexShader = renderDevice->CreateShaderModule(vertexShaderDescriptor);

    Cocoa::Graphics::ShaderModuleDesc pixelShaderDescriptor = {
        .shaderPath = "content/vertex.frag.spv"
    };
    auto pixelShader = renderDevice->CreateShaderModule(pixelShaderDescriptor);
    
    Cocoa::Graphics::BufferDesc mvpBufferDescriptor = {
        .usage = Cocoa::Graphics::GPUBufferUsage::Uniform,
        .size = sizeof(Cocoa::Graphics::MVP),
        .mapped = nullptr
    };
    auto mvpBuffer = renderDevice->CreateBuffer(mvpBufferDescriptor);

    Cocoa::Graphics::BindGroupLayoutEntry mvpLayout = {
        .binding = 0,
        .visibility = Cocoa::Graphics::GPUShaderStage::Vertex,
        .type = Cocoa::Graphics::GPUBindGroupType::UniformBuffer
    };

    Cocoa::Graphics::BindGroupLayoutDesc mvpBindGroupLayout = {
        .entries = {mvpLayout}
    };

    Cocoa::Graphics::BindGroupEntry mvpEntry = {
        .binding = 0,
        .buffer = mvpBuffer,
    };

    Cocoa::Graphics::BindGroupDesc mvpBindGroupDesc = {
        .layout = &mvpBindGroupLayout,
        .entries = {mvpEntry}
    };

    auto mvpBindGroup = renderDevice->CreateBindGroup(mvpBindGroupDesc);

    Cocoa::Graphics::PipelineLayoutDesc pipelineLayoutDesc = {
        .bindGroups = {mvpBindGroup}
    };

    auto pipelineLayout = renderDevice->CreatePipelineLayout(pipelineLayoutDesc);
    Cocoa::Graphics::PipelineDesc pipelineDescriptor{};
    pipelineDescriptor.AddShader(Cocoa::Graphics::GPUShaderStage::Vertex, vertexShader);
    pipelineDescriptor.AddShader(Cocoa::Graphics::GPUShaderStage::Pixel, pixelShader);
    pipelineDescriptor.Bind(0, sizeof(Cocoa::Graphics::Vertex))
                .Attribute(Cocoa::Graphics::GPUFormat::RGB32_SFLOAT, offsetof(Cocoa::Graphics::Vertex, pos))
                .Attribute(Cocoa::Graphics::GPUFormat::RGBA32_SFLOAT, offsetof(Cocoa::Graphics::Vertex, col));
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
            .clearColor = {0.6f, 0.21f, 0.36f, 1.0f},
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

        encoder->StartRendering(passDesc);
        encoder->SetRenderPipeline(renderPipeline);
        encoder->SetViewport(viewport);
        encoder->SetScissor(scissor);
        encoder->SetBindGroup(pipelineLayout, mvpBindGroup);
        encoder->SetVertexBuffer(vertexBuffer);
        encoder->SetIndexBuffer(indexBuffer);
        encoder->DrawIndexed(plane.indices.size(), 1, 0, 0, 0);
        encoder->EndRendering();

        renderDevice->EndEncoding(std::move(encoder));
    }

    std::cout << "Hello World!" << std::endl;
    renderDevice.reset();
    SDL_Quit();
    return 0;
}
