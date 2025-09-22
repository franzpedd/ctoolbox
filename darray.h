#ifndef DARRAY_INCLUDED
#define DARRAY_INCLUDED

#include <stddef.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// definitions and macros
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief handle shared/static library
#if defined(_WIN32) || defined(_WIN64)
    #if defined(CTOOLBOX_BUILD_SHARED)
        #define DARRAY_API __declspec(dllexport)
    #elif defined(CTOOLBOX_USE_SHARED)
        #define DARRAY_API __declspec(dllimport)
    #else
        #define DARRAY_API
    #endif
#else
    #if defined(CTOOLBOX_BUILD_SHARED) && (__GNUC__ >= 4)
        #define DARRAY_API __attribute__((visibility("default")))
    #else
        #define DARRAY_API
    #endif
#endif

/// @brief callback for custom allocation memories
typedef void* (*malloc_func)(size_t size);
typedef void* (*calloc_func)(size_t num, size_t size);
typedef void (*free_func)(void* ptr);
typedef void* (*realloc_func)(void* ptr, size_t newSize);

/// @brief bundle-up the memory functions into one structure
typedef struct darray_memfuncs_t
{
    malloc_func malloc_fn;
    calloc_func calloc_fn;
    free_func free_fn;
    realloc_func realloc_fn;
} darray_memfuncs;

/// @brief export memory functions
DARRAY_API extern const darray_memfuncs DARRAY_DEFAULT_MEMFUNCS;

/// @brief various types of erros that may happen when using the library
typedef enum darray_error_t
{
    DARRAY_SUCCESS = 0,
    DARRAY_ERROR_INVALID_PARAM,
    DARRAY_ERROR_MEMORY_ALLOC,
    DARRAY_ERROR_OUT_OF_BOUNDS,
    DARRAY_ERROR_EMPTY
} darray_error;

/// @brief opaque dynamic array structure
typedef struct darray darray;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/// @brief initializes the dynamic array
DARRAY_API darray_error darray_init(darray* outArray, size_t elementSize, size_t initialCapacity);

/// @brief initializes the dynamic array with custom memory allocation functions
DARRAY_API darray_error darray_init_memfuncs(darray* outArray, size_t elementSize, size_t initialCapacity, const darray_memfuncs* memfuncs);

/// @brief destroys the dynamic array, but not the content itself, be-warned
DARRAY_API void darray_destroy(darray* array);

/// @brief pushes an element into the array's back
DARRAY_API darray_error darray_push_back(darray* array, const void* element);

/// @brief removes an item from the array's back, not freeing it
DARRAY_API darray_error darray_pop_back(darray* array, void* elementOut);

/// @brief returns an item from the array
DARRAY_API darray_error darray_get(const darray* array, size_t index, void* elementOut);

/// @brief sets an index of the array to be the element
DARRAY_API darray_error darray_set(darray* array, size_t index, const void* element);

/// @brief inserts an item into a given position on the array
DARRAY_API darray_error darray_insert_at(darray* array, size_t index, const void* element);

/// @brief removes an item from a given position on the array
DARRAY_API darray_error darray_remove_at(darray* array, size_t index, void* elementOut);

/// @brief resizes the array to a new size
DARRAY_API darray_error darray_resize(darray* array, size_t newSize);

/// @brief reserves a certain ammout of items for the array
DARRAY_API darray_error darray_reserve(darray* array, size_t newCapacity);

/// @brief reallocates the array into a smaller memory area
DARRAY_API darray_error darray_shrink_to_fit(darray* array);

/// @brief returns the array's current size
DARRAY_API size_t darray_size(const darray* array);

/// @brief returns the array's current max capacity
DARRAY_API size_t darray_capacity(const darray* array);

/// @brief returns if the array is currently empty
DARRAY_API bool darray_empty(const darray* array);

#ifdef __cplusplus
}
#endif

#endif // DARRAY_INCLUDED
