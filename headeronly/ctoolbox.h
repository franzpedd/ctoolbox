#ifndef CTOOLBOX_INCLUDED
#define CTOOLBOX_INCLUDED

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Context
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Array
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief opaque dynamic array structure
typedef struct darray darray;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/// @brief initializes the dynamic array
CTOOLBOX_API darray* darray_init(size_t elementSize, size_t initialCapacity);

/// @brief initializes the dynamic array with custom memory allocation functions
CTOOLBOX_API darray* darray_init_memfuncs(size_t elementSize, size_t initialCapacity, const ctoolbox_memfuncs* memfuncs);

/// @brief destroys the dynamic array, but not the content itself, be-warned
CTOOLBOX_API void darray_destroy(darray* array);

/// @brief pushes an element into the array's back
CTOOLBOX_API ctoolbox_result darray_push_back(darray* array, const void* element);

/// @brief removes an item from the array's back, not freeing it
CTOOLBOX_API ctoolbox_result darray_pop_back(darray* array, void* elementOut);

/// @brief sees what is underneath a given index on the array
CTOOLBOX_API const void* darray_const_peek(const darray* array, size_t index);

/// @brief access the data underneath the array
CTOOLBOX_API const void* darray_const_data(const darray* array);

/// @brief returns an item from the array
CTOOLBOX_API ctoolbox_result darray_get(const darray* array, size_t index, void* elementOut);

/// @brief sets an index of the array to be the element
CTOOLBOX_API ctoolbox_result darray_set(darray* array, size_t index, const void* element);

/// @brief inserts an item into a given position on the array
CTOOLBOX_API ctoolbox_result darray_insert_at(darray* array, size_t index, const void* element);

/// @brief removes an item from a given position on the array
CTOOLBOX_API ctoolbox_result darray_remove_at(darray* array, size_t index, void* elementOut);

/// @brief resizes the array to a new size
CTOOLBOX_API ctoolbox_result darray_resize(darray* array, size_t newSize);

/// @brief reserves a certain ammout of items for the array
CTOOLBOX_API ctoolbox_result darray_reserve(darray* array, size_t newCapacity);

/// @brief reallocates the array into a smaller memory area
CTOOLBOX_API ctoolbox_result darray_shrink_to_fit(darray* array);

/// @brief returns the array's current size
CTOOLBOX_API size_t darray_size(const darray* array);

/// @brief returns the array's current max capacity
CTOOLBOX_API size_t darray_capacity(const darray* array);

/// @brief returns if the array is currently empty
CTOOLBOX_API bool darray_empty(const darray* array);

#ifdef __cplusplus
}
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ID Generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef IDGEN_MAX_SAFE_IDS
#define IDGEN_MAX_SAFE_IDS 16777216 // maximum of 16.7 million IDs (~2MB memory usage)
#endif // IDGEN_MAX_SAFE_IDS

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
extern "C" {
#endif

/// @brief opaque id generator structure
typedef struct idgen idgen;

/// @brief initializes generator, set to UINT32_MAX for full range
CTOOLBOX_API idgen* idgen_create(uint32_t start_id);

/// @brief initializes generator with custom allocation functions
CTOOLBOX_API idgen* idgen_create_memfuncs(uint32_t start_id, const ctoolbox_memfuncs* memfuncs);

/// @brief releases the resources of the id generator
CTOOLBOX_API void idgen_destroy(idgen* gen);

/// @brief generate next ID, returns 0 if exhausted
CTOOLBOX_API uint32_t idgen_next(idgen* gen);

/// @brief register an id
CTOOLBOX_API bool idgen_register(idgen* gen, uint32_t id);

/// @brief unregister an id
CTOOLBOX_API bool idgen_unregister(idgen* gen, uint32_t id);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static Hashtable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief defines the size of the static hash table
#ifndef SHASHTABLE_SIZE
    #define SHASHTABLE_SIZE 128
#endif

/// @brief opaque structure for the hash entry
typedef struct shash shash;

/// @brief opaque structure for the hash table
typedef struct shashtable shashtable;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/// @brief creates the hashtable
CTOOLBOX_API shashtable* shashtable_init();

/// @brief creates the hashtable with custom memory allocation functions
CTOOLBOX_API shashtable* shashtable_init_memfuncs(const ctoolbox_memfuncs* memfuncs);

/// @brief destroys the hashtable
CTOOLBOX_API void shashtable_destroy(shashtable* table);

/// @brief inserts an item into the hashtable
CTOOLBOX_API ctoolbox_result shashtable_insert(shashtable* table, const char* key, void* value);

/// @brief deletes an item from the hashtable
CTOOLBOX_API ctoolbox_result shashtable_delete(shashtable* table, const char* key);

/// @brief returns a peek at the given hash entry associated with given key
CTOOLBOX_API void* shashtable_lookup(shashtable* table, const char* key);

/// @brief checks if a given key exists in the hashtable
CTOOLBOX_API bool shashtable_contains(shashtable* table, const char* key);

/// @brief returns how many entries exists in the hashtable
CTOOLBOX_API size_t shashtable_count(shashtable* table);

#ifdef __cplusplus
}
#endif


#endif // CTOOLBOX_INCLUDED

#ifdef CTOOLBOX_IMPLEMENTATION
#undef CTOOLBOX_IMPLEMENTATION
#include "ctoolbox.c"
#endif // CTOOLBOX_IMPLEMENTATION
