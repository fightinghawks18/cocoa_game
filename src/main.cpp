#include <iostream>
#include <memory>
#include <filesystem>

#include <vulkan/vulkan.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "math/common.h"
#include "math/quaternion.h"
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

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        PANIC("Failed to start SDL3");
    }

    SDL_Window* window = SDL_CreateWindow("Cocoa", 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

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

    auto swapchainFormat = swapchain->GetFormat();
    vk::Format depthFormat = vk::Format::eD32Sfloat;

    auto currentDirectory = std::filesystem::current_path();
    auto vertexShaderCode = Cocoa::Vulkan::ReadFile((currentDirectory / "content" / "vertex.vert.spv").string());
    vk::ShaderModuleCreateInfo vertexShaderDescriptor;
    vertexShaderDescriptor.setCode(vertexShaderCode);

    auto vertexShader = std::make_unique<Cocoa::Vulkan::ShaderModule>(renderDevice.get(), vertexShaderDescriptor);

    auto pixelShaderCode = Cocoa::Vulkan::ReadFile((currentDirectory / "content" / "vertex.frag.spv").string());
    vk::ShaderModuleCreateInfo pixelShaderDescriptor;
    pixelShaderDescriptor.setCode(pixelShaderCode);
    auto pixelShader = std::make_unique<Cocoa::Vulkan::ShaderModule>(renderDevice.get(), pixelShaderDescriptor);

    auto position = Cocoa::Math::Vector3();
    auto rotation = Cocoa::Math::Quaternion();
    auto scale = Cocoa::Math::Vector3(1);

    auto model = Cocoa::Math::CreateModelMatrix(position, rotation, scale);
    auto projection = Cocoa::Math::CreatePerspectiveMatrix(Cocoa::Math::Radians(60), 1920.0f / 1080.0f, 0.1, 100);
    auto view = Cocoa::Math::LookAt(Cocoa::Math::Vector3(0, 0, 5), Cocoa::Math::Vector3(), Cocoa::Math::Vector3(0, 1, 0));

    Cocoa::Vulkan::MVP mvpData;
    mvpData.model = model.Transpose();
    mvpData.projection = projection.Transpose();
    mvpData.view = view.Transpose();
    
    Cocoa::Vulkan::BufferDesc mvpBufferDescriptor = {
        .usage = vk::BufferUsageFlagBits::eUniformBuffer,
        .size = sizeof(Cocoa::Vulkan::MVP),
        .mapped = &mvpData
    };
    auto mvpBuffer = std::make_unique<Cocoa::Vulkan::Buffer>(renderDevice.get(), mvpBufferDescriptor);

    Cocoa::Vulkan::BindGroupLayoutEntry mvpLayout = {
        .binding = 0,
        .visibility = Cocoa::Vulkan::GPUShaderStage::Vertex,
        .type = Cocoa::Vulkan::BindGroupType::UniformBuffer
    };

    Cocoa::Vulkan::BindGroupLayoutDesc mvpBindGroupLayout = {
        .entries = {mvpLayout}
    };

    Cocoa::Vulkan::BindGroupBuffer mvpBufferEntry = {
        .buffer = mvpBuffer->Get(),
        .offset = 0,
        .size = sizeof(Cocoa::Vulkan::MVP)
    };

    Cocoa::Vulkan::BindGroupEntry mvpEntry = {
        .binding = 0,
        .buffer = mvpBufferEntry,
    };

    Cocoa::Vulkan::BindGroupDesc mvpBindGroupDesc = {
        .layout = &mvpBindGroupLayout,
        .entries = {mvpEntry}
    };

    auto mvpBindGroup = std::make_unique<Cocoa::Vulkan::BindGroup>(renderDevice.get(), mvpBindGroupDesc);
    auto layouts = { mvpBindGroup->GetLayout() };

    vk::PipelineLayoutCreateInfo pipelineLayoutDescriptor;
    pipelineLayoutDescriptor.setSetLayouts(layouts)
                .setPushConstantRanges(nullptr);

    auto pipelineLayout = std::make_unique<Cocoa::Vulkan::PipelineLayout>(renderDevice.get(), pipelineLayoutDescriptor);

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
        vk::PipelineShaderStageCreateInfo(
            {},
            vk::ShaderStageFlagBits::eVertex,
            vertexShader->Get(),
            "main"
        ),
        vk::PipelineShaderStageCreateInfo(
            {},
            vk::ShaderStageFlagBits::eFragment,
            pixelShader->Get(),
            "main"
        )
    };

    vk::VertexInputAttributeDescription posAttribute(
        0,
        0,
        vk::Format::eR32G32B32Sfloat,
        offsetof(Cocoa::Vulkan::Vertex, pos)
    );

    vk::VertexInputAttributeDescription colAttribute(
        1,
        0,
        vk::Format::eR32G32B32A32Sfloat,
        offsetof(Cocoa::Vulkan::Vertex, col)
    );

    std::vector attributes = {
        posAttribute,
        colAttribute
    };

    vk::VertexInputBindingDescription vertexBinding(
        0,
        sizeof(Cocoa::Vulkan::Vertex),
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
                .setLayout(pipelineLayout->Get())
                .setRenderPass(nullptr)
                .setSubpass(0);

    auto renderPipeline = std::make_unique<Cocoa::Vulkan::RenderPipeline>(renderDevice.get(), renderPipelineDescriptor);

    auto plane = Cocoa::Vulkan::CreatePlaneIndexed();

    Cocoa::Vulkan::BufferDesc vertexBufferDescriptor = {
        .usage = vk::BufferUsageFlagBits::eVertexBuffer,
        .size = sizeof(Cocoa::Vulkan::Vertex) * plane.vertices.size(),
        .mapped = plane.vertices.data()
    };
    auto vertexBuffer = std::make_unique<Cocoa::Vulkan::Buffer>(renderDevice.get(), vertexBufferDescriptor);

    Cocoa::Vulkan::BufferDesc indexBufferDescriptor = {
        .usage = vk::BufferUsageFlagBits::eIndexBuffer,
        .size = sizeof(uint16_t) * plane.indices.size(),
        .mapped = plane.indices.data()
    };
    auto indexBuffer = std::make_unique<Cocoa::Vulkan::Buffer>(renderDevice.get(), indexBufferDescriptor);

    bool gameRun = true;
    while (gameRun) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                gameRun = false;
                break;
            }
        }

        rotation *= Cocoa::Math::FromAxisAngle(Cocoa::Math::Vector3(1, 0, 0), Cocoa::Math::Radians(0.5));
        rotation.Normalize();
        model = Cocoa::Math::CreateModelMatrix(position, rotation, scale);
        
        swapchain->GetNextBackBuffer();
        auto swapchainExtent = swapchain->GetExtent();

        projection = Cocoa::Math::CreatePerspectiveMatrix(Cocoa::Math::Radians(60), static_cast<float>(swapchainExtent.width) / static_cast<float>(swapchainExtent.height), 0.1, 100);

        mvpData.model = model.Transpose();
        mvpData.projection = projection.Transpose();
        mvpData.view = view.Transpose();
        mvpBuffer->MapTo(&mvpData, sizeof(Cocoa::Vulkan::MVP), 0);
        
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
                    .setRenderArea({{0, 0}, swapchainExtent})
                    .setViewMask(0)
                    .setLayerCount(1);

        vk::Rect2D scissor;
        scissor.setOffset({0, 0})
                .setExtent(swapchainExtent);
        vk::Viewport viewport;
        viewport.setWidth(swapchainExtent.width)
                .setHeight(swapchainExtent.height)
                .setMaxDepth(1)
                .setMinDepth(0)
                .setX(0)
                .setY(0);

        encoder->StartRendering(renderingDescriptor);
        encoder->SetRenderPipeline(renderPipeline.get());
        encoder->SetViewport(viewport);
        encoder->SetScissor(scissor);
        encoder->SetBindGroup(pipelineLayout.get(), mvpBindGroup.get());
        encoder->SetVertexBuffer(vertexBuffer.get());
        encoder->SetIndexBuffer(indexBuffer.get());
        encoder->DrawIndexed(plane.indices.size(), 1, 0, 0, 0);
        encoder->EndRendering();

        renderDevice->EndEncoding(std::move(encoder));
    }

    std::cout << "Hello World!" << std::endl;
    
    renderPipeline.reset();
    pipelineLayout.reset();

    mvpBindGroup.reset();
    mvpBuffer.reset();
    vertexShader.reset();
    pixelShader.reset();
    vertexBuffer.reset();
    indexBuffer.reset();

    swapchain.reset();
    surface.reset();
    renderDevice.reset();
    SDL_Quit();
    return 0;
}
