#include "buffer.h"
#include "../device.h"

#include "../../macros.h"
#include "../internal/helpers/flags.h"
#include "../internal/helpers/enums.h"

namespace Cocoa::Vulkan {
    Buffer::Buffer(Device* device, Graphics::GFXBufferDesc desc) : _device(device) {
        vk::BufferCreateInfo bufferDescriptor{};
        bufferDescriptor.setUsage(GPUBufferUsageToVk(desc.usage))
                    .setSharingMode(vk::SharingMode::eExclusive)
                    .setSize(desc.size);
        VmaAllocationCreateInfo allocationDescriptor = {0};
        allocationDescriptor.usage = VMA_MEMORY_USAGE_AUTO;
        allocationDescriptor.flags = GPUMemoryAccessToVma(desc.access);

        VmaAllocationInfo allocationInfo;
        VkBuffer buffer;
        VkResult result = vmaCreateBuffer(
            _device->GetAllocator(), 
            reinterpret_cast<const VkBufferCreateInfo*>(&bufferDescriptor), 
            &allocationDescriptor, 
            &buffer, 
            &_allocation, 
            &allocationInfo
        );
        if (result != VK_SUCCESS) {
            PANIC("Failed to create a vulkan buffer");
        }
        
        _buffer = buffer;
        _size = desc.size;
        _mapped = allocationInfo.pMappedData;

        if (desc.mapped != nullptr) {
            MapTo(desc.mapped, desc.size, 0);
        }
    }

    Buffer::~Buffer() {
        _device->GetDevice().waitIdle();
        vmaDestroyBuffer(_device->GetAllocator(), _buffer, _allocation);
    }

    void Buffer::MapTo(void* mapped, uint64_t size, uint64_t offset) {
        memcpy(static_cast<char*>(_mapped) + offset, mapped, size);
    }
}