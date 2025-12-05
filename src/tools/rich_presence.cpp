#include "rich_presence.h"
#include <ctime>

namespace Cocoa::Tools
{
    RichPresence::RichPresence( RichPresenceDesc desc )
    {
        DiscordEventHandlers handlers = { };
        Discord_Initialize( desc.appID.c_str() , &handlers , 1 , nullptr );
        p_RichPresence = { 0 };
    }

    void RichPresence::Update()
    {
        Discord_RunCallbacks();
    }

    void RichPresence::SetState( const std::string& state )
    {
        p_RichPresence.state = state.c_str();
        Discord_UpdatePresence( &p_RichPresence );
    }

    void RichPresence::SetStateDetailed( const std::string& details )
    {
        p_RichPresence.details = details.c_str();
        Discord_UpdatePresence( &p_RichPresence );
    }

    void RichPresence::StartTimestamp()
    {
        p_RichPresence.startTimestamp = time( nullptr );
        Discord_UpdatePresence( &p_RichPresence );
    }

    void RichPresence::StopTimestamp( float sFromNow )
    {
        p_RichPresence.endTimestamp = sFromNow == 0 ? 0 : sFromNow;
        Discord_UpdatePresence( &p_RichPresence );
    }

    void RichPresence::SetLargeImage( const std::string& large )
    {
        p_RichPresence.largeImageKey = large.c_str();
        Discord_UpdatePresence( &p_RichPresence );
    }

    void RichPresence::SetLargeImageTip( const std::string& tip )
    {
        p_RichPresence.largeImageText = tip.c_str();
        Discord_UpdatePresence( &p_RichPresence );
    }

    void RichPresence::SetSmallImage( const std::string& small )
    {
        p_RichPresence.smallImageKey = small.c_str();
        Discord_UpdatePresence( &p_RichPresence );
    }

    void RichPresence::SetSmallImageTip( const std::string& tip )
    {
        p_RichPresence.smallImageKey = tip.c_str();
        Discord_UpdatePresence( &p_RichPresence );
    }

    RichPresence::~RichPresence()
    {
        Discord_Shutdown();
    }
}
