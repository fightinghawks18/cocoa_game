#include <filesystem>
#include <iostream>
#include <memory>

#include <SDL3/SDL.h>

#include "directX12/core/render_device.h"
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

    SDL_Window* window = SDL_CreateWindow(  "Cocoa" , 800 , 600 , SDL_WINDOW_RESIZABLE );

    Cocoa::Graphics::RenderDeviceDesc deviceDesc = {
        .desiredQueues = {
            Cocoa::Graphics::GPUQueueType::Graphics,
            Cocoa::Graphics::GPUQueueType::Transfer
        }
    };
    auto directX12Impl = std::make_unique<Cocoa::D3D12::RenderDevice>( deviceDesc );
    auto renderDevice = std::unique_ptr<>(  );
    auto renderWindow = renderDevice->ConnectWindow( {
        .window = window
    } );

    // Rich presence (for fun)
    const Cocoa::Tools::RichPresenceDesc rpcDescriptor = {
        .appID = "1444737693090316409"
    };
    Cocoa::Tools::RichPresence rpc( rpcDescriptor );
    rpc.SetState( "Watching the void" );
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

        Cocoa::Graphics::RenderEncoderDesc encoderDesc = {
            .swapChain = &renderWindow ,
            .submitQueue = Cocoa::Graphics::GPUQueueType::Graphics
        };

        auto encoder = renderDevice->Encode( encoderDesc );
        encoder.StartRenderPass( { .renderArea = {{0, 0}, {1, 1}} } );
        encoder.EndRenderPass();
        renderDevice->EndEncoding( encoder );

    }

    renderDevice->WaitForIdle();
    renderDevice.reset();
    SDL_Quit();
    return 0;
}
