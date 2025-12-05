#include <filesystem>
#include <iostream>
#include <memory>

#include <SDL3/SDL.h>

#include "directX12/core/render_device_impl.h"
#include "graphics/core/render_device.h"
#include "tools/rich_presence.h"

#include "macros.h"

constexpr float sensitivity = 0.1f;

int main()
{
    if ( !SDL_Init( SDL_INIT_VIDEO ) )
    {
        PANIC( "Failed to start SDL3" );
    }

    SDL_Window* window = SDL_CreateWindow( "Cocoa" , 800 , 600 , SDL_WINDOW_VULKAN );

    auto directX12Impl = std::make_unique<Cocoa::D3D12::RenderDeviceImpl>();
    const auto renderDevice = std::make_unique<Cocoa::Graphics::RenderDevice>( std::move(directX12Impl) );
    const auto renderWindow = renderDevice->ConnectWindow( {
        .window = window
    } );

    // Rich presence (for fun)
    Cocoa::Tools::RichPresenceDesc rpcDescriptor = {
        .appID = "1444737693090316409"
    };
    Cocoa::Tools::RichPresence rpc( rpcDescriptor );
    rpc.SetState( "Watching a plane" );
    rpc.StartTimestamp();

    bool gameRun = true;
    while ( gameRun )
    {
        SDL_Event e;
        while ( SDL_PollEvent( &e ) )
        {
            if ( e.type == SDL_EVENT_QUIT )
            {
                gameRun = false;
                break;
            }
        }

        // Update RPC
        rpc.Update();
    }

    std::cout << "Hello World!" << std::endl;
    SDL_Quit();
    return 0;
}
