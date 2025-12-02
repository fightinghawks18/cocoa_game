#include "render_pipeline.h"
#include "../device.h"

#include "../internal/helpers/enums.h"
#include "../internal/helpers/flags.h"

namespace Cocoa::Vulkan {
    RenderPipeline::RenderPipeline(Device* device, Graphics::PipelineDesc desc) : _device(device) {
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
        shaderStages.reserve(desc.shaders.size());
        for (const auto& stage : desc.shaders) {
            auto shader = device->GetShaderModuleInstance(stage.second)->Get();
            vk::PipelineShaderStageCreateInfo stageDesc{};
            stageDesc.setStage(GPUShaderStageToVkBit(stage.first))
                    .setModule(shader)
                    .setPName("main");
            shaderStages.push_back(stageDesc);
        }

        std::vector<vk::VertexInputAttributeDescription> attributes;
        std::vector<vk::VertexInputBindingDescription> bindings;
        for (const auto& binding : desc.vertexLayout) {
            vk::VertexInputBindingDescription vertexBinding(
                binding.binding,
                binding.stride,
                vk::VertexInputRate::eVertex
            );

            for (uint32_t i = 0; const auto& attribute : binding.attributes) {
                vk::VertexInputAttributeDescription vertexAttribute(
                    i,
                    binding.binding,
                    GPUColorFormatToVk(attribute.format),
                    attribute.offset
                );
                i++;
                attributes.push_back(vertexAttribute);
            }

            bindings.push_back(vertexBinding);
        }

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.setVertexAttributeDescriptions(attributes)
                    .setVertexBindingDescriptions(bindings);

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
            {},
            GPUTopologyToVk(desc.topology),
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
            GPUPolygonModeToVk(desc.polygonMode),
            GPUCullModeToVk(desc.cullMode),
            GPUFrontFaceToVk(desc.frontFace),
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

        vk::StencilOpState frontFaceState(
                GPUStencilOpToVk(desc.stencilFrontFace.failOp),
                GPUStencilOpToVk(desc.stencilFrontFace.passOp),
                GPUStencilOpToVk(desc.stencilFrontFace.depthFailOp),
                GPUCompareOpToVk(desc.stencilFrontFace.compareOp),
                desc.stencilFrontFace.compareMask,
                desc.stencilFrontFace.writeMask,
                desc.stencilFrontFace.reference
        );

        vk::StencilOpState backFaceState(
                GPUStencilOpToVk(desc.stencilBackFace.failOp),
                GPUStencilOpToVk(desc.stencilBackFace.passOp),
                GPUStencilOpToVk(desc.stencilBackFace.depthFailOp),
                GPUCompareOpToVk(desc.stencilBackFace.compareOp),
                desc.stencilBackFace.compareMask,
                desc.stencilBackFace.writeMask,
                desc.stencilBackFace.reference
        );

        vk::PipelineDepthStencilStateCreateInfo depthStencil(
            {},
            desc.depthTest,
            desc.depthWrite,
            GPUCompareOpToVk(desc.depthCompareOp),
            false,
            desc.stencilTest,
            frontFaceState,
            backFaceState,
            0.0f, 1.0f
        );

        auto colorFormat = GPUColorFormatToVk(desc.colorFormat);
        auto depthStencilFormat = GPUDepthStencilFormatToVk(desc.depthStencilFormat);
        vk::Format stencilFormat;
        switch (desc.depthStencilFormat) {
            case Graphics::GPUDepthStencilFormat::Unknown:
            case Graphics::GPUDepthStencilFormat::DepthFloat32_NoStencil:
            case Graphics::GPUDepthStencilFormat::DepthUnorm16_NoStencil:
                stencilFormat = vk::Format::eUndefined;
                break;
            case Graphics::GPUDepthStencilFormat::DepthUnorm24_StencilUint8:
            case Graphics::GPUDepthStencilFormat::DepthFloat32_StencilUint8:
            case Graphics::GPUDepthStencilFormat::DepthUnorm16_StencilUint8:
                stencilFormat = depthStencilFormat;
                break;
        }

        vk::PipelineRenderingCreateInfo pipelineRenderingInfo(
            0,
            1,
            &colorFormat,
            depthStencilFormat,
            stencilFormat
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
                    .setLayout(device->GetPipelineLayoutInstance(desc.pipelineLayout)->Get())
                    .setRenderPass(nullptr)
                    .setSubpass(0);
        _renderPipeline = device->GetDevice().createGraphicsPipelineUnique(nullptr, renderPipelineDescriptor).value;
    }

    RenderPipeline::~RenderPipeline() {
        _device->GetDevice().waitIdle();
        _renderPipeline.reset();
    }
}