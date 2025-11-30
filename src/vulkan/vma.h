#pragma once

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

#ifdef _WIN32
    #include <vma/vk_mem_alloc.h>
#else
    #include <vk_mem_alloc.h>
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif