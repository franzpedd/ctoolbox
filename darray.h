#ifndef DARRAY_INCLUDED
#define DARRAY_INCLUDED

#include "context.h"

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

#endif // DARRAY_INCLUDED
