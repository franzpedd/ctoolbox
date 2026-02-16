#include "context.h"
#include <stdlib.h>

#ifdef __cplusplus
CTOOLBOX_API const ctoolbox_memfuncs CTOOLBOX_DEFAULT_MEMFUNCS =
{
    malloc,   /* .malloc_fn */
    calloc,   /* .calloc_fn */
    free,     /* .free_fn */
    realloc   /* .realloc_fn */
};
#else
CTOOLBOX_API const ctoolbox_memfuncs CTOOLBOX_DEFAULT_MEMFUNCS =
{
    .malloc_fn = malloc,
    .calloc_fn = calloc,
    .free_fn = free,
    .realloc_fn = realloc
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

CTOOLBOX_API void* ctoolbox_custom_malloc(const ctoolbox_memfuncs* fun, size_t size)
{
    return fun->malloc_fn ? fun->malloc_fn(size) : malloc(size);
}

CTOOLBOX_API void* ctoolbox_custom_calloc(const ctoolbox_memfuncs* fun, size_t num, size_t size)
{
    return fun->calloc_fn ? fun->calloc_fn(num, size) : calloc(num, size);
}

CTOOLBOX_API void ctoolbox_custom_free(const ctoolbox_memfuncs* fun, void* ptr)
{
    fun->free_fn ? fun->free_fn(ptr) : free(ptr);
}

CTOOLBOX_API void* ctoolbox_custom_realloc(const ctoolbox_memfuncs* fun, void* ptr, size_t newSize)
{
    return fun->realloc_fn ? fun->realloc_fn(ptr, newSize) : realloc(ptr, newSize);
}

#ifdef __cplusplus
}
#endif
