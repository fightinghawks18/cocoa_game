#include "buffer.h"
#include "../device.h"

#include "../../macros.h"
#include "../internal/helpers/flags.h"

namespace Cocoa::Vulkan {
    Buffer::Buffer(Device* device, Graphics::BufferDesc desc) : _device(device) {
        vk::BufferCreateInfo bufferDescriptor{};
        bufferDescriptor.setUsage(GPUBufferUsageToVk(desc.usage))
                    .setSharingMode(vk::SharingMode::eExclusive)
                    .setSize(desc.size);
        VmaAllocationCreateInfo allocationDescriptor = {0};
        allocationDescriptor.usage = VMA_MEMORY_USAGE_AUTO;
        allocationDescriptor.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        VkBuffer buffer;
        VkResult result = vmaCreateBuffer(
            _device->GetAllocator(), 
            reinterpret_cast<const VkBufferCreateInfo*>(&bufferDescriptor), 
            &allocationDescriptor, 
            &buffer, 
            &_allocation, 
            nullptr
        );
        if (result != VK_SUCCESS) {
            PANIC("Failed to create a vulkan buffer");
        }
        
        _buffer = buffer;
        _size = desc.size;

        if (desc.mapped != nullptr) {
            MapTo(desc.mapped, desc.size, 0);
        }
    }

    Buffer::~Buffer() {
        _device->GetDevice().waitIdle();
        vmaDestroyBuffer(_device->GetAllocator(), _buffer, _allocation);
    }

    void Buffer::MapTo(void* mapped, uint64_t size, uint64_t offset) {
        void* data;
        vmaMapMemory(_device->GetAllocator(), _allocation, &data);
        memcpy(static_cast<char*>(data) + offset, mapped, size);
        vmaUnmapMemory(_device->GetAllocator(), _allocation);
    }
}