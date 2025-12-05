#pragma once

#include <string>
#include <discord_rpc.h>

namespace Cocoa::Tools {
    struct RichPresenceDesc {
        std::string appID;
    };

    /// @brief Integrates Discord's rich presence feature
    class RichPresence { 
    public:
        RichPresence(RichPresenceDesc desc);
        ~RichPresence();

        /// @brief Updates the rich presence, displaying it on Discord
        void Update();

        /// @brief Briefly describes what you're doing
        /// @param state The text to display on Discord
        void SetState(const std::string& state);

        /// @brief Adds additional info
        /// @param details The text to display under the state on Discord
        void SetStateDetailed(const std::string& details);

        /// @brief Sets the starting timestamp
        /// @note This is used for restarting/starting the timer for the elapsed timer on Discord
        void StartTimestamp();

        /// @brief Sets the time that the activity should end
        /// @param sFromNow Seconds from now that this will end
        /// @note Setting -1 will disable the end timestamp
        void StopTimestamp(float sFromNow = -1);

        /// @brief Sets the large image displayed on Discord
        /// @param large URL to a large Discord image, or an external one
        void SetLargeImage(const std::string& large);

        /// @brief Sets the text that appears when hovering over the large image
        /// @param tip The message to display
        void SetLargeImageTip(const std::string& tip);

        /// @brief Sets the small image displayed on Discord
        /// @param small URL to a small Discord image, or an external one
        void SetSmallImage(const std::string& small);

        /// @brief Sets the text that appears when hovering over the small image
        /// @param tip The message to display
        void SetSmallImageTip(const std::string& tip);
    private:
        DiscordRichPresence p_RichPresence;
    };
}