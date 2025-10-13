#ifndef CTOOLBOX_CONTEXT_INCLUDED
#define CTOOLBOX_CONTEXT_INCLUDED

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/// @brief compilation options
#if defined(CTOOLBOX_BUILD_SHARED) // shared library
    #if defined(_WIN32) || defined(_WIN64)
        #if defined(CTOOLBOX_EXPORTS)
            #define CTOOLBOX_API __declspec(dllexport)
        #else
            #define CTOOLBOX_API __declspec(dllimport)
        #endif
    #elif defined(__linux__) && !defined(__ANDROID__)
        #define CTOOLBOX_API __attribute__((visibility("default")))
    #else
        #define CTOOLBOX_API
    #endif
#else
    #define CTOOLBOX_API // static library
#endif

/// @brief various types of erros that may happen when using the library
typedef enum ctoolbox_result
{
    CTOOLBOX_SUCCESS = 0,
    CTOOLBOX_ERROR_INVALID_PARAM,
    CTOOLBOX_ERROR_MEMORY_ALLOC,
    CTOOLBOX_ERROR_OUT_OF_BOUNDS,
    CTOOLBOX_ERROR_EMPTY,
    CTOOLBOX_ERROR_NOT_FOUND,
    CTOOLBOX_ERROR_FULL
} ctoolbox_result;

/// @brief callback for custom allocation memories
typedef void* (*malloc_func)(size_t size);
typedef void* (*calloc_func)(size_t num, size_t size);
typedef void (*free_func)(void* ptr);
typedef void* (*realloc_func)(void* ptr, size_t newSize);

/// @brief bundle-up the memory functions into one structure
typedef struct ctoolbox_memfuncs
{
    malloc_func malloc_fn;
    calloc_func calloc_fn;
    free_func free_fn;
    realloc_func realloc_fn;
} ctoolbox_memfuncs;

/// @brief export memory functions
CTOOLBOX_API extern const ctoolbox_memfuncs CTOOLBOX_DEFAULT_MEMFUNCS;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/// @brief custom malloc call, overrides default malloc with an optional custom function provided by the user
CTOOLBOX_API void* ctoolbox_custom_malloc(const ctoolbox_memfuncs* fun, size_t size);

/// @brief custom calloc call, overrides default calloc with an optional custom function provided by the user
CTOOLBOX_API void* ctoolbox_custom_calloc(const ctoolbox_memfuncs* fun, size_t num, size_t size);

/// @brief custom free call, overrides default free with an optional custom function provided by the user
CTOOLBOX_API void ctoolbox_custom_free(const ctoolbox_memfuncs* fun, void* ptr);

/// @brief custom realloc call, overrides default realloc with an optional custom function provided by the user
CTOOLBOX_API void* ctoolbox_custom_realloc(const ctoolbox_memfuncs* fun, void* ptr, size_t newSize);

#ifdef __cplusplus
}
#endif

#endif // CTOOLBOX_CONTEXT_INCLUDED