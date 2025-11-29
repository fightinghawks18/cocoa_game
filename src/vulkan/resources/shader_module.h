#pragma once

#include <vector>
#include <fstream>
#include <vulkan/vulkan.hpp>

#include "../../macros.h"

namespace Cocoa::Vulkan {
    inline std::vector<uint32_t> ReadFile(const std::string& path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            PANIC("Failed to open shader file %s", path.c_str());
        }
        
        auto fileSize = static_cast<size_t>(file.tellg());
        std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();

        return buffer;
    }

    class Device;
    class ShaderModule {
    public:
        ShaderModule(Device* device, vk::ShaderModuleCreateInfo desc);
        ~ShaderModule();

        [[nodiscard]] vk::ShaderModule Get() { return _shaderMod.get(); }
    private:
        Device* _device;
        
        vk::UniqueShaderModule _shaderMod;
    };
}