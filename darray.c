#include "darray.h"

#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// internal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct darray
{
	void* data;
    size_t size;
    size_t capacity;
    size_t elementSize;
    darray_memfuncs memfuncs;
};  

DARRAY_API const darray_memfuncs DARRAY_DEFAULT_MEMFUNCS =
{
    .malloc_fn = malloc,
    .calloc_fn = calloc,
    .free_fn = free,
    .realloc_fn = realloc
};

DARRAY_API void* darray_malloc(const darray* darray, size_t size)
{
    return darray->memfuncs.malloc_fn ? darray->memfuncs.malloc_fn(size) : malloc(size);
}

DARRAY_API void* darray_calloc(const darray* darray, size_t num, size_t size)
{
    return darray->memfuncs.calloc_fn ? darray->memfuncs.calloc_fn(num, size) : calloc(num, size);
}

DARRAY_API void darray_free(const darray* darray, void* ptr)
{
    if (darray->memfuncs.free_fn) darray->memfuncs.free_fn(ptr);
    else free(ptr);
}

DARRAY_API void* darray_realloc(const darray* array, void* ptr, size_t newSize)
{
    if (array->memfuncs.realloc_fn) return array->memfuncs.realloc_fn(ptr, newSize);
    
    // fallback: no realloc available
    void* newMem = darray_malloc(array, newSize);
    if (newMem && ptr)
    {
        // return NULL to tell caller we can't realloc
        darray_free(array, ptr);
    }
    return newMem;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// external
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DARRAY_API darray_error darray_init(darray* outArray, size_t elementSize, size_t initialCapacity)
{
    return darray_init_memfuncs(outArray, elementSize, initialCapacity, &DARRAY_DEFAULT_MEMFUNCS);
}

DARRAY_API darray_error darray_init_memfuncs(darray* outArray, size_t elementSize, size_t initialCapacity, const darray_memfuncs* memfuncs)
{
    if (!outArray || elementSize == 0) return DARRAY_ERROR_INVALID_PARAM;

    memset(outArray, 0, sizeof(*outArray));
    outArray->size = 0;
    outArray->elementSize = elementSize;
    outArray->capacity = initialCapacity > 0 ? initialCapacity : 1;

    if (memfuncs) outArray->memfuncs = *memfuncs;
    else outArray->memfuncs = DARRAY_DEFAULT_MEMFUNCS;

    outArray->data = darray_calloc(outArray, outArray->capacity, elementSize);
    if (!outArray->data) return DARRAY_ERROR_MEMORY_ALLOC;

    return DARRAY_SUCCESS;
}

DARRAY_API void darray_destroy(darray* array)
{
    darray_free(array, array->data);

    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
}

DARRAY_API darray_error darray_push_back(darray* array, const void* element)
{
    if (!array || !element) return DARRAY_ERROR_INVALID_PARAM;

    // ensure capacity
    if (array->size >= array->capacity) {
        darray_error res = darray_reserve(array, array->capacity * 2);
        if (res != DARRAY_SUCCESS) return res;
    }

    // add new element
    memcpy((char*)array->data + (array->size * array->elementSize), element, array->elementSize);
    array->size++;

    return DARRAY_SUCCESS;
}

DARRAY_API darray_error darray_pop_back(darray* array, void* elementOut)
{
    if (!array || array->size == 0) return DARRAY_ERROR_EMPTY;
    if (elementOut) memcpy(elementOut, (char*)array->data + ((array->size - 1) * array->elementSize), array->elementSize);

    array->size--;
    return DARRAY_SUCCESS;
}

DARRAY_API darray_error darray_get(const darray *array, size_t index, void* elementOut)
 {
    if (!array || !elementOut || index >= array->size) return DARRAY_ERROR_INVALID_PARAM;
    memcpy(elementOut, (char*)array->data + (index * array->elementSize), array->elementSize);
    return DARRAY_SUCCESS;
}

DARRAY_API darray_error darray_set(darray* array, size_t index, const void* element)
 {
    if (!array || !element || index >= array->size) return DARRAY_ERROR_INVALID_PARAM;
    memcpy((char*)array->data + (index * array->elementSize), element, array->elementSize);
    return DARRAY_SUCCESS;
}

DARRAY_API darray_error darray_insert_at(darray* array, size_t index, const void* element)
{
    if (!array || !element || index > array->size) return DARRAY_ERROR_INVALID_PARAM;

    // ensure capacity
    if (array->size >= array->capacity) {
        darray_error result = darray_reserve(array, array->capacity * 2);
        if (result != DARRAY_SUCCESS) return result;
    }

    // shift elements to make space
    if (index < array->size) {
        char* data = (char*)array->data;
        memmove(data + ((index + 1) * array->elementSize), data + (index * array->elementSize), (array->size - index) * array->elementSize);
    }

    // insert new element
    memcpy((char*)array->data + (index * array->elementSize), element, array->elementSize);
    array->size++;
    return DARRAY_SUCCESS;
}

DARRAY_API darray_error darray_remove_at(darray* array, size_t index, void* elementOut)
{
    if (!array || index >= array->size) {
        return DARRAY_ERROR_INVALID_PARAM;
    }

    if (array->size == 0) {
        return DARRAY_ERROR_EMPTY;
    }

    // if the caller wants the removed element, copy it first
    if (elementOut) {
        memcpy(elementOut, (char*)array->data + (index * array->elementSize), array->elementSize);
    }

    // shift elements left to overwrite the removed element
    if (index < array->size - 1) {
        char* data = (char*)array->data;
        memmove(data + (index * array->elementSize), data + ((index + 1) * array->elementSize), (array->size - index - 1) * array->elementSize);
    }

    array->size--;
    return DARRAY_SUCCESS;
}

DARRAY_API darray_error darray_resize(darray* array, size_t newSize)
{
    if (!array) return DARRAY_ERROR_INVALID_PARAM;
    if (newSize > array->capacity) {
        darray_error res = darray_reserve(array, newSize);
        if (res != DARRAY_SUCCESS) return res;
    }
    
    array->size = newSize;
    return DARRAY_SUCCESS;
}

DARRAY_API darray_error darray_reserve(darray *array, size_t newCapacity)
{
    if (!array) return DARRAY_ERROR_INVALID_PARAM;
    if (newCapacity <= array->capacity) return DARRAY_SUCCESS;

    size_t newSizeBytes = newCapacity * array->elementSize;
    size_t oldSizeBytes = array->size * array->elementSize;

    void* newData = NULL;

    // try realloc directly
    if (array->memfuncs.realloc_fn) {
        newData = array->memfuncs.realloc_fn(array->data, newSizeBytes);
        if (!newData) return DARRAY_ERROR_MEMORY_ALLOC;
    } 

    // fallback: allocate, copy and free
    else {
        newData = darray_malloc(array, newSizeBytes);
        if (!newData) return DARRAY_ERROR_MEMORY_ALLOC;

        memcpy(newData, array->data, oldSizeBytes);
        darray_free(array, array->data);
    }

    array->data = newData;
    array->capacity = newCapacity;

    return DARRAY_SUCCESS;
}

DARRAY_API darray_error darray_shrink_to_fit(darray* array)
{
    if (!array) return DARRAY_ERROR_INVALID_PARAM;
    if (array->size == array->capacity) return DARRAY_SUCCESS;

    if (array->size == 0) {
        darray_free(array, array->data);
        array->data = NULL;
        array->capacity = 0;
        return DARRAY_SUCCESS;
    }

    void* newData = darray_malloc(array, array->size * array->elementSize);
    if (!newData) return DARRAY_ERROR_MEMORY_ALLOC;

    memcpy(newData, array->data, array->size * array->elementSize);
    darray_free(array, array->data);
    array->data = newData;
    array->capacity = array->size;
    return DARRAY_SUCCESS;
}

DARRAY_API size_t darray_size(const darray* array)
 {
    return array ? array->size : 0;
}

DARRAY_API size_t darray_capacity(const darray* array)
 {
    return array ? array->capacity : 0;
}

DARRAY_API bool darray_empty(const darray *array)
 {
    return array ? (array->size == 0) : true;
}