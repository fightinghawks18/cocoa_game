#include "rich_presence.h"
#include <ctime>

namespace Cocoa::Tools {
    RichPresence::RichPresence(RichPresenceDesc desc) {
        DiscordEventHandlers handlers = {};
        Discord_Initialize(desc.appID.c_str(), &handlers, 1, nullptr);
        _richPresence = {0};
    }

    void RichPresence::Update() {
        Discord_RunCallbacks();
    }

    void RichPresence::SetState(const std::string& state) {
        _richPresence.state = state.c_str();
        Discord_UpdatePresence(&_richPresence);
    }

    void RichPresence::SetStateDetailed(const std::string& details) {
        _richPresence.details = details.c_str();
        Discord_UpdatePresence(&_richPresence);
    }

    void RichPresence::StartTimestamp() {
        _richPresence.startTimestamp = time(nullptr);
        Discord_UpdatePresence(&_richPresence);
    }

    void RichPresence::StopTimestamp(float sFromNow) {
        _richPresence.endTimestamp = sFromNow == 0 ? 0 : sFromNow;
        Discord_UpdatePresence(&_richPresence);
    }

    void RichPresence::SetLargeImage(const std::string& large) {
        _richPresence.largeImageKey = large.c_str();
        Discord_UpdatePresence(&_richPresence);
    }

    void RichPresence::SetLargeImageTip(const std::string& tip) {
        _richPresence.largeImageText = tip.c_str();
        Discord_UpdatePresence(&_richPresence);
    }

    void RichPresence::SetSmallImage(const std::string& small) {
        _richPresence.smallImageKey = small.c_str();
        Discord_UpdatePresence(&_richPresence);
    }

    void RichPresence::SetSmallImageTip(const std::string& tip) {
        _richPresence.smallImageKey = tip.c_str();
        Discord_UpdatePresence(&_richPresence);
    }

    RichPresence::~RichPresence() {
        Discord_Shutdown();
    }
}