#pragma once

#include <stdexcept>
#include <format>

#ifdef DEBUG
    #include "debug/log.h"
    #define PUSH_INFO(str, ...) \
        Cocoa::Debug::Log::Push(Cocoa::Debug::LogType::Info, str __VA_OPT__(,) __VA_ARGS__)

    #define PUSH_WARN(str, ...) \
        Cocoa::Debug::Log::Push(Cocoa::Debug::LogType::Warn, str __VA_OPT__(,) __VA_ARGS__)

    #define PUSH_ERR(str, ...) \
        Cocoa::Debug::Log::Push(Cocoa::Debug::LogType::Error, str __VA_OPT__(,) __VA_ARGS__)
#else
    #define PUSH_INFO(str, ...)
    #define PUSH_WARN(str, ...)
    #define PUSH_ERR(str, ...)
#endif

#define PANIC(str, ...) \
    do { \
        PUSH_ERR(str __VA_OPT__(,) __VA_ARGS__); \
        fprintf(stderr, "Cocoa hit an unrecoverable exception!\n"); \
        fprintf(stderr, str "\n" __VA_OPT__(,) __VA_ARGS__); \
        throw std::runtime_error("Cocoa hit an unrecoverable exception!"); \
    } while ( 0 )

#define THROW_IF_FAILED(cond, ...) \
    if (cond) PANIC( __VA_ARGS__ )