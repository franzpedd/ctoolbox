# CToolbox
Useful C data structures bundled-up into one library;

User owns the memory, not the library;
Available both as a dynamic or static library;
Supports custom memory allocator, uses default if not provided;
Currently not thread-safe;

### darray (dynamic array)
* darray_init(); / darray_init_memfuncs();
* darray_destroy();
* darray_push_back();
* darray_pop_back();
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

### shashtable (static hashtable)

* shashtable_init(); / shashtable_init_memfuncs();
* shashtable_destroy();
* shashtable_insert();
* shashtable_delete()
* shashtable_lookup();
* shashtable_contains();
* shashtable_count();

Capacity is statically set at 128 but can be overwritten with ```#define SHASHTABLE_SIZE```. Handles collisions with a linked list.

## License