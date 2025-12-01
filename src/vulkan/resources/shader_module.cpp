#include "shader_module.h"

#include <filesystem>
#include "../device.h"

namespace Cocoa::Vulkan {
    ShaderModule::ShaderModule(Device* device, Graphics::ShaderModuleDesc desc) : _device(device) {
        auto currentDirectory = std::filesystem::current_path();
        auto vertexShaderCode = Cocoa::Vulkan::ReadFile((currentDirectory / desc.shaderPath).string());
        vk::ShaderModuleCreateInfo vertexShaderDescriptor;
        vertexShaderDescriptor.setCode(vertexShaderCode);
        _shaderMod = device->GetDevice().createShaderModuleUnique(vertexShaderDescriptor);
    }

    ShaderModule::~ShaderModule() {
        _device->GetDevice().waitIdle();
        _shaderMod.reset();
    }
}