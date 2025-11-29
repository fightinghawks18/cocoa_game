#include "shader_module.h"
#include "../device.h"

namespace Cocoa::Vulkan {
    ShaderModule::ShaderModule(Device* device, vk::ShaderModuleCreateInfo desc) : _device(device) {
        _shaderMod = device->GetDevice().createShaderModuleUnique(desc);
    }

    ShaderModule::~ShaderModule() {
        _device->GetDevice().waitIdle();
        _shaderMod.reset();
    }
}