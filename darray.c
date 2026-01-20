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
    ctoolbox_memfuncs memfuncs;
};  

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// external
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTOOLBOX_API darray* darray_init(size_t elementSize, size_t initialCapacity)
{
    return darray_init_memfuncs(elementSize, initialCapacity, &CTOOLBOX_DEFAULT_MEMFUNCS);
}

CTOOLBOX_API darray* darray_init_memfuncs(size_t elementSize, size_t initialCapacity, const ctoolbox_memfuncs* memfuncs)
{
    if (elementSize == 0) return NULL;

    darray* outArray = ctoolbox_custom_malloc(memfuncs ? memfuncs : &CTOOLBOX_DEFAULT_MEMFUNCS, sizeof(darray));
    if (!outArray) return NULL;

    memset(outArray, 0, sizeof(darray));
    outArray->size = 0;
    outArray->elementSize = elementSize;
    outArray->capacity = initialCapacity > 0 ? initialCapacity : 1;

    if (memfuncs) outArray->memfuncs = *memfuncs;
    else outArray->memfuncs = CTOOLBOX_DEFAULT_MEMFUNCS;

    outArray->data = ctoolbox_custom_calloc(&outArray->memfuncs, outArray->capacity, elementSize);
    if (!outArray->data) return NULL;

    return outArray;
}

CTOOLBOX_API void darray_destroy(darray* array)
{
    if(!array) return;
    ctoolbox_custom_free(&array->memfuncs, array->data);
    ctoolbox_custom_free(&array->memfuncs, array);
}

CTOOLBOX_API ctoolbox_result darray_push_back(darray* array, const void* element)
{
    if (!array) return CTOOLBOX_ERROR_INVALID_PARAM;

    // ensure capacity
    if (array->size >= array->capacity) {
        ctoolbox_result res = darray_reserve(array, array->capacity * 2);
        if (res != CTOOLBOX_SUCCESS) return res;
    }

    // add new element
    memcpy((char*)array->data + (array->size * array->elementSize), element, array->elementSize);
    array->size++;

    return CTOOLBOX_SUCCESS;
}

CTOOLBOX_API ctoolbox_result darray_pop_back(darray* array, void* elementOut)
{
    if (!array || array->size == 0) return CTOOLBOX_ERROR_EMPTY;
    if (elementOut) memcpy(elementOut, (char*)array->data + ((array->size - 1) * array->elementSize), array->elementSize);

    array->size--;
    return CTOOLBOX_SUCCESS;
}

CTOOLBOX_API const void* darray_const_peek(const darray* array, size_t index)
{
    if (!array || index >= array->size) return NULL;
    return (char*)array->data + (index * array->elementSize);
}

CTOOLBOX_API const void* darray_const_data(const darray* array)
{
    return array ? array->data : NULL;
}

CTOOLBOX_API ctoolbox_result darray_get(const darray *array, size_t index, void* elementOut)
 {
    if (!array || !elementOut || index >= array->size) return CTOOLBOX_ERROR_INVALID_PARAM;
    memcpy(elementOut, (char*)array->data + (index * array->elementSize), array->elementSize);
    return CTOOLBOX_SUCCESS;
}

CTOOLBOX_API ctoolbox_result darray_set(darray* array, size_t index, const void* element)
 {
    if (!array || !element || index >= array->size) return CTOOLBOX_ERROR_INVALID_PARAM;
    memcpy((char*)array->data + (index * array->elementSize), element, array->elementSize);
    return CTOOLBOX_SUCCESS;
}

CTOOLBOX_API ctoolbox_result darray_insert_at(darray* array, size_t index, const void* element)
{
    if (!array || !element || index > array->size) return CTOOLBOX_ERROR_INVALID_PARAM;

    // ensure capacity
    if (array->size >= array->capacity) {
        ctoolbox_result result = darray_reserve(array, array->capacity * 2);
        if (result != CTOOLBOX_SUCCESS) return result;
    }

    // shift elements to make space
    if (index < array->size) {
        char* data = (char*)array->data;
        memmove(data + ((index + 1) * array->elementSize), data + (index * array->elementSize), (array->size - index) * array->elementSize);
    }

    // insert new element
    memcpy((char*)array->data + (index * array->elementSize), element, array->elementSize);
    array->size++;
    return CTOOLBOX_SUCCESS;
}

CTOOLBOX_API ctoolbox_result darray_remove_at(darray* array, size_t index, void* elementOut)
{
    if (!array || index >= array->size) {
        return CTOOLBOX_ERROR_INVALID_PARAM;
    }

    if (array->size == 0) {
        return CTOOLBOX_ERROR_EMPTY;
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
    return CTOOLBOX_SUCCESS;
}

CTOOLBOX_API ctoolbox_result darray_resize(darray* array, size_t newSize)
{
    if (!array) return CTOOLBOX_ERROR_INVALID_PARAM;
    if (newSize > array->capacity) {
        ctoolbox_result res = darray_reserve(array, newSize);
        if (res != CTOOLBOX_SUCCESS) return res;
    }
    
    array->size = newSize;
    return CTOOLBOX_SUCCESS;
}

CTOOLBOX_API ctoolbox_result darray_reserve(darray* array, size_t newCapacity)
{
    if (!array) return CTOOLBOX_ERROR_INVALID_PARAM;
    if (newCapacity <= array->capacity) return CTOOLBOX_SUCCESS;

    size_t newSizeBytes = newCapacity * array->elementSize;
    size_t oldSizeBytes = array->size * array->elementSize;

    void* newData = NULL;

    // try realloc directly
    if (array->memfuncs.realloc_fn) {
        newData = ctoolbox_custom_realloc(&array->memfuncs, array->data, newSizeBytes);
        if (!newData) return CTOOLBOX_ERROR_MEMORY_ALLOC;
    } 

    // fallback: allocate, copy and free
    else {
        newData = ctoolbox_custom_malloc(&array->memfuncs, newSizeBytes);
        if (!newData) return CTOOLBOX_ERROR_MEMORY_ALLOC;

        memcpy(newData, array->data, oldSizeBytes);
        ctoolbox_custom_free(&array->memfuncs, array->data);
    }

    array->data = newData;
    array->capacity = newCapacity;

    return CTOOLBOX_SUCCESS;
}

CTOOLBOX_API ctoolbox_result darray_shrink_to_fit(darray* array)
{
    if (!array) return CTOOLBOX_ERROR_INVALID_PARAM;
    if (array->size == array->capacity) return CTOOLBOX_SUCCESS;

    if (array->size == 0) {
        ctoolbox_custom_free(&array->memfuncs, array->data);
        array->data = NULL;
        array->capacity = 0;
        return CTOOLBOX_SUCCESS;
    }

    void* newData = ctoolbox_custom_malloc(&array->memfuncs, array->size * array->elementSize);
    if (!newData) return CTOOLBOX_ERROR_MEMORY_ALLOC;

    memcpy(newData, array->data, array->size * array->elementSize);
    ctoolbox_custom_free(&array->memfuncs, array->data);
    array->data = newData;
    array->capacity = array->size;
    return CTOOLBOX_SUCCESS;
}

CTOOLBOX_API size_t darray_size(const darray* array)
 {
    return array ? array->size : 0;
}

CTOOLBOX_API size_t darray_capacity(const darray* array)
 {
    return array ? array->capacity : 0;
}

CTOOLBOX_API bool darray_empty(const darray *array)
 {
    return array ? (array->size == 0) : true;
}
