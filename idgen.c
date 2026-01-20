#include "idgen.h"

#include <stdlib.h>
#include <string.h>

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
