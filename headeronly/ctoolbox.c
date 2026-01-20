#include "ctoolbox.h"
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Context
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTOOLBOX_API const ctoolbox_memfuncs CTOOLBOX_DEFAULT_MEMFUNCS =
{
    .malloc_fn = malloc,
    .calloc_fn = calloc,
    .free_fn = free,
    .realloc_fn = realloc
};

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Array
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ID Generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct idgen
{
    uint32_t current_id;
    uint32_t start_id;
    uint32_t max_id;
    uint32_t count;
    uint32_t bitset_size;     // number of uint32_t words
    uint32_t* used_bits;      // bitset representing used IDs
    const ctoolbox_memfuncs* memfuncs;
};

// macros for bit manipulation
#define BIT_INDEX(id, base)   ((id) - (base))
#define BIT_WORD(i)           ((i) >> 5)          // divide by 32
#define BIT_MASK(i)           (1u << ((i) & 31u))

static inline bool bit_test(uint32_t* bits, uint32_t bitset_size, uint32_t idx)
{
    uint32_t word = BIT_WORD(idx);
    if (word >= bitset_size) return false;
    return bits[word] & BIT_MASK(idx);
}

static inline void bit_set(uint32_t* bits, uint32_t bitset_size, uint32_t idx)
{
    uint32_t word = BIT_WORD(idx);
    if (word >= bitset_size) return;
    bits[word] |= BIT_MASK(idx);
}

static inline void bit_clear(uint32_t* bits, uint32_t bitset_size, uint32_t idx)
{
    uint32_t word = BIT_WORD(idx);
    if (word >= bitset_size) return;
    bits[word] &= ~BIT_MASK(idx);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// external
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTOOLBOX_API idgen* idgen_create(uint32_t start_id)
{
    return idgen_create_memfuncs(start_id, &CTOOLBOX_DEFAULT_MEMFUNCS);
}

CTOOLBOX_API idgen* idgen_create_memfuncs(uint32_t start_id, const ctoolbox_memfuncs* memfuncs)
{
    const ctoolbox_memfuncs* actual_memfuncs = memfuncs ? memfuncs : &CTOOLBOX_DEFAULT_MEMFUNCS;
    if (start_id >= IDGEN_MAX_SAFE_IDS) return NULL;

    idgen* gen = ctoolbox_custom_malloc(actual_memfuncs, sizeof(idgen));
    if (!gen)  return NULL;

    memset(gen, 0, sizeof(*gen));
    gen->start_id = start_id;
    gen->current_id = start_id;
    gen->max_id = IDGEN_MAX_SAFE_IDS;
    gen->memfuncs = actual_memfuncs;

    gen->bitset_size = (IDGEN_MAX_SAFE_IDS + 31u) / 32u;
    if (gen->bitset_size == 0) {
        gen->bitset_size = 1;
    }

    gen->used_bits = ctoolbox_custom_malloc(actual_memfuncs, gen->bitset_size * sizeof(uint32_t));
    if (!gen->used_bits) {
        ctoolbox_custom_free(actual_memfuncs, gen);
        return NULL;
    }

    memset(gen->used_bits, 0, gen->bitset_size * sizeof(uint32_t));
    return gen;
}

void idgen_destroy(idgen* gen)
{
    if (!gen) return;
    const ctoolbox_memfuncs* mem = gen->memfuncs;
    if (gen->used_bits) ctoolbox_custom_free(mem, gen->used_bits);
    ctoolbox_custom_free(mem, gen);
}

CTOOLBOX_API uint32_t idgen_next(idgen* gen)
{
    if (!gen) return 0;

    uint32_t range = gen->max_id - gen->start_id;
    for (uint32_t i = 0; i < range; i++) {
        uint32_t candidate = gen->current_id + i;
        if (candidate >= gen->max_id)
            candidate = gen->start_id + (candidate - gen->max_id);

        uint32_t idx = BIT_INDEX(candidate, gen->start_id);
        if (!bit_test(gen->used_bits, gen->bitset_size, idx)) {  // Add bounds check
            bit_set(gen->used_bits, gen->bitset_size, idx);      // Add bounds check
            gen->count++;
            gen->current_id = candidate + 1;
            if (gen->current_id >= gen->max_id)
                gen->current_id = gen->start_id;
            return candidate;
        }
    }
    return 0;
}

CTOOLBOX_API bool idgen_register(idgen* gen, uint32_t id)
{
    if (!gen || id < gen->start_id || id >= gen->max_id) return false;

    uint32_t idx = BIT_INDEX(id, gen->start_id);
    if (bit_test(gen->used_bits, gen->bitset_size, idx)) return false;  // Add bounds check

    bit_set(gen->used_bits, gen->bitset_size, idx);  // Add bounds check
    gen->count++;
    return true;
}

CTOOLBOX_API bool idgen_unregister(idgen* gen, uint32_t id)
{
    if (!gen || id < gen->start_id || id >= gen->max_id) return false;

    uint32_t idx = BIT_INDEX(id, gen->start_id);
    if (!bit_test(gen->used_bits, gen->bitset_size, idx)) return false;

    bit_clear(gen->used_bits, gen->bitset_size, idx);
    gen->count--;

    // move current_id back for better reuse
    if (id < gen->current_id) gen->current_id = id;

    return true;
}

CTOOLBOX_API bool idgen_is_registered(idgen* gen, uint32_t id)
{
    if (!gen || id < gen->start_id || id >= gen->max_id) return false;
    uint32_t idx = BIT_INDEX(id, gen->start_id);
    return bit_test(gen->used_bits, gen->bitset_size, idx);
}

CTOOLBOX_API uint32_t idgen_count(idgen* gen)
{
    return gen ? gen->count : 0;
}

CTOOLBOX_API void idgen_reset(idgen* gen)
{
    if (!gen) return;
    memset(gen->used_bits, 0, gen->bitset_size * sizeof(uint32_t));
    gen->count = 0;
    gen->current_id = gen->start_id;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static Hashtable
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// internal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct shash 
{
    char* key;
    void* value;
    struct shash* next;
};

struct shashtable
{
    shash* buckets[SHASHTABLE_SIZE];
    ctoolbox_memfuncs memfuncs;
    size_t count;  // Track total entries
};

CTOOLBOX_API unsigned long shash_djb2_hash(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = (hash << 5) + hash + c;  // hash * 33 + c (original djb2)
        hash ^= (hash << 7) | (hash >> (sizeof(hash) * 8 - 7)); // additional mixing
    }

    return hash % SHASHTABLE_SIZE;
}

CTOOLBOX_API ctoolbox_result shash_strdup(const ctoolbox_memfuncs* fun, const char* src, char** output)
{
    if (!fun || !src || !output) return CTOOLBOX_ERROR_INVALID_PARAM;
    
    size_t len = strlen(src);
    char* str = (char*)ctoolbox_custom_malloc(fun, len + 1);
    if (!str) return CTOOLBOX_ERROR_MEMORY_ALLOC;
    
    memcpy(str, src, len + 1);
    *output = str;
    
    return CTOOLBOX_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// external
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTOOLBOX_API shashtable* shashtable_init()
{
   return shashtable_init_memfuncs(&CTOOLBOX_DEFAULT_MEMFUNCS);
}

CTOOLBOX_API shashtable* shashtable_init_memfuncs(const ctoolbox_memfuncs* memfuncs)
{
    shashtable* outHashtable = ctoolbox_custom_malloc(memfuncs ? memfuncs : &CTOOLBOX_DEFAULT_MEMFUNCS, sizeof(shashtable));

    for (int i = 0; i < SHASHTABLE_SIZE; i++) {
        outHashtable->buckets[i] = NULL;
    }
    
    if (memfuncs) outHashtable->memfuncs = *memfuncs;
    else outHashtable->memfuncs = CTOOLBOX_DEFAULT_MEMFUNCS;

    outHashtable->count = 0;
    return outHashtable;
}

CTOOLBOX_API void shashtable_destroy(shashtable* table)
{
    if (!table) return;

    for (int i = 0; i < SHASHTABLE_SIZE; i++) {
        shash* entry = table->buckets[i];
        while (entry) {
            shash* next = entry->next;
            ctoolbox_custom_free(&table->memfuncs, entry->key);
            ctoolbox_custom_free(&table->memfuncs, entry);
            entry = next;
        }
    }
    ctoolbox_custom_free(&table->memfuncs, table);
}

CTOOLBOX_API ctoolbox_result shashtable_insert(shashtable *table, const char* key, void* value)
{
    if (!table || !key) return CTOOLBOX_ERROR_INVALID_PARAM;
    if (table->count >= SHASHTABLE_SIZE * 10) return CTOOLBOX_ERROR_FULL;

    unsigned long index = shash_djb2_hash(key);
    
    // check if key already exists
    shash* current = table->buckets[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return CTOOLBOX_SUCCESS;
        }
        current = current->next;
    }

    // create new entry
    shash* new_entry = (shash*)ctoolbox_custom_malloc(&table->memfuncs, sizeof(shash));
    if (!new_entry) return CTOOLBOX_ERROR_MEMORY_ALLOC;

    new_entry->key = NULL;
    new_entry->value = value;
    new_entry->next = NULL;

    // duplicate key
    ctoolbox_result result = shash_strdup(&table->memfuncs, key, &new_entry->key);
    if (result != CTOOLBOX_SUCCESS) {
        ctoolbox_custom_free(&table->memfuncs, new_entry);
        return result;
    }

    // insert at beginning of chain (simplest)
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    table->count++;

    return CTOOLBOX_SUCCESS;
}

CTOOLBOX_API ctoolbox_result shashtable_delete(shashtable* table, const char* key)
{
    if (!table || !key) return CTOOLBOX_ERROR_INVALID_PARAM;

    unsigned long index = shash_djb2_hash(key);
    shash* current = table->buckets[index];
    shash* prev = NULL;

    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (prev) prev->next = current->next;
            else table->buckets[index] = current->next;

            ctoolbox_custom_free(&table->memfuncs, current->key);
            ctoolbox_custom_free(&table->memfuncs, current);
            table->count--;

            return CTOOLBOX_SUCCESS;
        }
        prev = current;
        current = current->next;
    }

    return CTOOLBOX_ERROR_NOT_FOUND;
}

CTOOLBOX_API void* shashtable_lookup(shashtable* table, const char* key)
{
    if (!table || !key) return NULL;

    unsigned long index = shash_djb2_hash(key);
    const shash* current = table->buckets[index];

    while (current) {
        if (strcmp(current->key, key) == 0) return current->value;
        current = current->next;
    }

    return NULL;
}

CTOOLBOX_API bool shashtable_contains(shashtable* table, const char *key)
{
    return shashtable_lookup(table, key) != NULL;
}

CTOOLBOX_API size_t shashtable_count(shashtable* table)
{
    return table ? table->count : 0;
}

