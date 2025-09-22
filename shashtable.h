#ifndef SHASHTABLE_INCLUDED
#define SHASHTABLE_INCLUDED

#include "context.h"

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
CTOOLBOX_API ctoolbox_result shashtable_init(shashtable* outHashtable);

/// @brief creates the hashtable with custom memory allocation functions
CTOOLBOX_API ctoolbox_result shashtable_init_memfuncs(shashtable* outHashtable, const ctoolbox_memfuncs* memfuncs);

/// @brief destroys the hashtable
CTOOLBOX_API void shashtable_destroy(shashtable* table);

/// @brief inserts an item into the hashtable
CTOOLBOX_API ctoolbox_result shashtable_insert(shashtable* table, const char* key, void* value);

/// @brief deletes an item from the hashtable
CTOOLBOX_API ctoolbox_result shashtable_delete(shashtable* table, const char* key);

/// @brief returns a peek at the given hash entry associated with given key
CTOOLBOX_API void* shashtable_lookup(shashtable* table, const char* key);

/// @brief checks if a given key exists in the hashtable
CTOOLBOX_API bool shashtable_contains(const shashtable* table, const char* key);

/// @brief returns how many entries exists in the hashtable
CTOOLBOX_API size_t shashtable_count(const shashtable* table);

#ifdef __cplusplus
}
#endif

#endif // SHASHTABLE_INCLUDED
