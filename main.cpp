#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error("Failed to start SDL3");
    }

    SDL_Window* window = SDL_CreateWindow("Cocoa", 800, 600, SDL_WINDOW_VULKAN);

    vk::ApplicationInfo appDescriptor;
    appDescriptor.setApplicationVersion(VK_MAKE_API_VERSION(0, 1, 0, 0))
                .setPApplicationName("Cocoa")
                .setEngineVersion(VK_MAKE_API_VERSION(0, 1, 0, 0))
                .setPEngineName("Cocoa")
                .setApiVersion(VK_MAKE_API_VERSION(0, 1, 3, 0));

    uint32_t sdlCount;
    auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlCount);
    
    vk::InstanceCreateFlags instanceFlags;

    #ifdef __APPLE__
        instanceFlags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
    #endif

    vk::InstanceCreateInfo instanceDescriptor;
    instanceDescriptor.setPApplicationInfo(&appDescriptor)
                    .setEnabledExtensionCount(sdlCount)
                    .setPpEnabledExtensionNames(sdlExtensions)
                    .setFlags(instanceFlags);
    
    auto instance = vk::createInstanceUnique(instanceDescriptor);

    auto physicalDevices = instance->enumeratePhysicalDevices();
    auto gpu = physicalDevices[0];

    std::vector<const char*> deviceExtensions = {
        #ifdef __APPLE__
            "VK_KHR_portability_subset"
        #endif
    };

    vk::DeviceCreateInfo deviceDescriptor;
    deviceDescriptor.setPEnabledExtensionNames(deviceExtensions);
    auto device = gpu.createDeviceUnique(deviceDescriptor);

    bool gameRun = true;
    while (gameRun) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                gameRun = false;
                break;
            }
        }
    }

    std::cout << "Hello World!" << std::endl;

    device.reset();
    instance.reset();

    SDL_Quit();
    return 0;
}
