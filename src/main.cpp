#include <iostream>
#include <memory>
#include <filesystem>

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
    
    Cocoa::Vulkan::SwapchainDesc swapchainDesc = {
        .surface = surface
    };
    auto swapchain = renderDevice->CreateSwapchain(swapchainDesc);
    auto swapchainInstance = renderDevice->GetSwapchainInstance(swapchain);

    auto swapchainFormat = swapchainInstance->GetFormat();
    vk::Format depthFormat = vk::Format::eD32Sfloat;

    auto currentDirectory = std::filesystem::current_path();
    auto vertexShaderCode = Cocoa::Vulkan::ReadFile((currentDirectory / "content" / "vertex.vert.spv").string());
    vk::ShaderModuleCreateInfo vertexShaderDescriptor;
    vertexShaderDescriptor.setCode(vertexShaderCode);

    auto vertexShader = renderDevice->CreateShaderModule(vertexShaderDescriptor);

    auto pixelShaderCode = Cocoa::Vulkan::ReadFile((currentDirectory / "content" / "vertex.frag.spv").string());
    vk::ShaderModuleCreateInfo pixelShaderDescriptor;
    pixelShaderDescriptor.setCode(pixelShaderCode);
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
        .type = Cocoa::Graphics::BindGroupType::UniformBuffer
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
    auto layouts = { renderDevice->GetBindGroupInstance(mvpBindGroup)->GetLayout() };

    vk::PipelineLayoutCreateInfo pipelineLayoutDescriptor;
    pipelineLayoutDescriptor.setSetLayouts(layouts)
                .setPushConstantRanges(nullptr);

    auto pipelineLayout = renderDevice->CreatePipelineLayout(pipelineLayoutDescriptor);

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
        vk::PipelineShaderStageCreateInfo(
            {},
            vk::ShaderStageFlagBits::eVertex,
            renderDevice->GetShaderModuleInstance(vertexShader)->Get(),
            "main"
        ),
        vk::PipelineShaderStageCreateInfo(
            {},
            vk::ShaderStageFlagBits::eFragment,
            renderDevice->GetShaderModuleInstance(pixelShader)->Get(),
            "main"
        )
    };

    vk::VertexInputAttributeDescription posAttribute(
        0,
        0,
        vk::Format::eR32G32B32Sfloat,
        offsetof(Cocoa::Graphics::Vertex, pos)
    );

    vk::VertexInputAttributeDescription colAttribute(
        1,
        0,
        vk::Format::eR32G32B32A32Sfloat,
        offsetof(Cocoa::Graphics::Vertex, col)
    );

    std::vector attributes = {
        posAttribute,
        colAttribute
    };

    vk::VertexInputBindingDescription vertexBinding(
        0,
        sizeof(Cocoa::Graphics::Vertex),
        vk::VertexInputRate::eVertex
    );
    
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.setVertexAttributeDescriptions(attributes)
                .setVertexBindingDescriptions(vertexBinding);

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
        {},
        vk::PrimitiveTopology::eTriangleList,
        VK_FALSE
    );

    vk::PipelineViewportStateCreateInfo viewportState(
        {},
        1, nullptr,
        1, nullptr
    );

    vk::PipelineRasterizationStateCreateInfo rasterizer(
        {},
        VK_FALSE,  
        VK_FALSE,
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eNone,
        vk::FrontFace::eCounterClockwise,
        VK_FALSE,
        0.0f, 0.0f, 0.0f,
        1.0f  
    );

    vk::PipelineMultisampleStateCreateInfo multisampling(
        {},
        vk::SampleCountFlagBits::e1,
        VK_FALSE,
        1.0f,
        nullptr,
        VK_FALSE,
        VK_FALSE
    );

    vk::PipelineColorBlendAttachmentState colorBlendAttachment(
        VK_FALSE,
        vk::BlendFactor::eOne,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::BlendFactor::eOne,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd,
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | 
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    );

    vk::PipelineColorBlendStateCreateInfo colorBlending(
        {},
        VK_FALSE,
        vk::LogicOp::eCopy,
        1, &colorBlendAttachment,
        {0.0f, 0.0f, 0.0f, 0.0f} 
    );

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicState(
        {},
        dynamicStates.size(),
        dynamicStates.data()
    );

    vk::PipelineDepthStencilStateCreateInfo depthStencil(
        {},
        VK_TRUE,
        VK_TRUE,
        vk::CompareOp::eLess,
        VK_FALSE,
        VK_FALSE,
        {},
        {},
        0.0f, 1.0f
    );


    vk::PipelineRenderingCreateInfo pipelineRenderingInfo(
        0,
        1,
        &swapchainFormat,
        depthFormat,
        vk::Format::eUndefined
    );

    vk::GraphicsPipelineCreateInfo renderPipelineDescriptor{};
    renderPipelineDescriptor.setStages(shaderStages)
                .setPNext(&pipelineRenderingInfo)
                .setPVertexInputState(&vertexInputInfo)
                .setPInputAssemblyState(&inputAssembly)
                .setPTessellationState(nullptr)
                .setPViewportState(&viewportState)
                .setPRasterizationState(&rasterizer)
                .setPMultisampleState(&multisampling)
                .setPDepthStencilState(&depthStencil)
                .setPColorBlendState(&colorBlending)
                .setPDynamicState(&dynamicState)
                .setLayout(renderDevice->GetPipelineLayoutInstance(pipelineLayout)->Get())
                .setRenderPass(nullptr)
                .setSubpass(0);

    auto renderPipeline = renderDevice->CreateRenderPipeline(renderPipelineDescriptor);

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
