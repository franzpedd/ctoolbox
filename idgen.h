#ifndef IDGEN_H
#define IDGEN_H

#include "context.h"

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

/// @brief check if an ID is currently registered
CTOOLBOX_API bool idgen_is_registered(idgen* gen, uint32_t id);

/// @brief get the number of currently registered IDs
CTOOLBOX_API uint32_t idgen_count(idgen* gen);

/// @brief resets generator to initial state
CTOOLBOX_API void idgen_reset(idgen* gen);

#ifdef __cplusplus
}
#endif

#endif // IDGEN_H