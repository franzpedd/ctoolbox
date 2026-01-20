# CToolbox
Useful C data structures bundled-up into one library;

User owns the memory, not the library;
Available as a dynamic, static library or header only;
Supports custom memory allocator, uses default if not provided;
Not thread-safe;

### darray (dynamic array)
* darray_init(); / darray_init_memfuncs();
* darray_destroy();
* darray_push_back();
* darray_pop_back();
* darray_const_peek();
* darray_const_data();
* darray_get();
* darray_set(); 
* darray_insert_at();
* darray_remove_at();
* darray_resize();
* darray_reserve();
* darray_shrink_to_fit();
* darray_size();
* darray_capacity();
* darray_empty();

### idgen  (id generator)

* idgen_create(); / idgen_create_memfuncs();
* idgen_destroy();
* idgen_next();
* idgen_register();
* idgen_unregister();
* idgen_is_registered();
* idgen_count();
* idgen_reset();

### shashtable (static hashtable)

* shashtable_init(); / shashtable_init_memfuncs();
* shashtable_destroy();
* shashtable_insert();
* shashtable_delete()
* shashtable_lookup();
* shashtable_contains();
* shashtable_count();

Capacity is statically set at 128 but can be overwritten with ```#define SHASHTABLE_SIZE```. Handles collisions with a linked list.

## Header only
There's a C++ generator for the header-only version, it creates the files ```headeronly/ctoolbox.h``` and ```headeronly/ctoolbox.c```. Here's how to use it:

* Just ```#define CTOOLBOX_IMPLEMENTATION``` in one .c (source) file on your project before ```#include "ctoolbox.h``` and you're set. Both files must be present on your project's directory path but you should not compile ```ctoolbox.c``` in this case.