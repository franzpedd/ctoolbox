#include "shashtable.h"

#include <string.h>

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

CTOOLBOX_API ctoolbox_result shashtable_init(shashtable* outHashtable)
{
   return shashtable_init_memfuncs(outHashtable, &CTOOLBOX_DEFAULT_MEMFUNCS);
}

CTOOLBOX_API ctoolbox_result shashtable_init_memfuncs(shashtable* outHashtable, const ctoolbox_memfuncs* memfuncs)
{
    if (!outHashtable) return CTOOLBOX_ERROR_INVALID_PARAM;

    memset(outHashtable, 0, sizeof(*outHashtable));
    for (int i = 0; i < SHASHTABLE_SIZE; i++) {
        outHashtable->buckets[i] = NULL;
    }
    
    if (memfuncs) outHashtable->memfuncs = *memfuncs;
    else outHashtable->memfuncs = CTOOLBOX_DEFAULT_MEMFUNCS;

    outHashtable->count = 0;
    return CTOOLBOX_SUCCESS;
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

CTOOLBOX_API bool shashtable_contains(const shashtable *table, const char *key)
{
    return shashtable_lookup(table, key) != NULL;
}

CTOOLBOX_API size_t shashtable_count(const shashtable *table)
{
    return table ? table->count : 0;
}