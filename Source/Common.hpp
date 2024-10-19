#pragma once

#include <functional>

#ifdef _WIN32
    #ifdef VOIDENGINE_EXPORTS
        #define VOIDENGINE_API __declspec(dllexport)
    #else
        #define VOIDENGINE_API __declspec(dllimport)
    #endif
#else
    #define VOIDENGINE_API
#endif

#ifndef ENGINE_DIR
    #define ENGINE_DIR "../"
#endif

namespace VoidEngine
{
    // from: https://stackoverflow.com/a/57595105
    template <typename T, typename... Rest>
    void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };
}