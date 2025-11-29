#pragma once

#include <cstdio>
#include <string>

namespace Cocoa::Debug {
    enum class LogType {
        Info,
        Warning,
        Error
    };

    class Log {
    public:
        template <typename... A>
        static inline void Push(LogType type, const std::string& message, A&&... args) {
            FILE* stream;
            switch (type) {
                case LogType::Info:
                    stream = stdout;
                    break;
                case LogType::Warning:
                case LogType::Error:
                    stream = stderr;
                    break;
            }

            fprintf(stream, "[Cocoa]:");
            if constexpr (sizeof...(args) > 0) {
                fprintf(stream, message.c_str(), std::forward<A>(args)...);
            } else {
                fprintf(stream, "%s", message.c_str());
            }
            fprintf(stream, "\n");
        }
    };
}