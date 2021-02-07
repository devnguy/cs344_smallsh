#include <assert.h>
#include <stdlib.h>
#include "DynArr.h"

struct dynarr_t
{
    TYPE *data;
    int size;
    int capacity;
};

/**
 * Initialize (including allocation of data) a dynamic array.
 */
void dynarr_init(struct dynarr_t *da, int capacity)
{
    assert(capacity > 0);
    assert(da != 0);
    da->data = (TYPE *) malloc(sizeof(TYPE) * capacity);
    assert(da->data != 0);
    da->size = 0;
    da->capacity = capacity;
}

/**
 * Allocate and initialize dynamic array.
 */
struct dynarr_t *dynarr_create(int cap)
{
    assert(cap > 0);
    struct dynarr_t *da = (struct dynarr_t *)malloc(sizeof( struct dynarr_t));
    assert(da != 0);
    dynarr_init(da, cap);
    return da;
}

/**
 * Deallocate data array in dynamic array.
 */
void dynarr_free(struct dynarr_t *da)
{
    if(da->data != 0) {
        free(da->data);
        da->data = 0;
    }
    da->size = 0;
    da->capacity = 0;
}

/**
 * Deallocate data array and the dynamic array ure. 
 */
void dynarr_destroy(struct dynarr_t *da)
{
    dynarr_free(da);
    free(da);
}

/**
 * Resizes the underlying array to be the size cap 
 */
void _dynarr_set_cap(struct dynarr_t *da, int new_cap)
{
    // Allocate an array of size new_cap.
    TYPE *temp = (TYPE*)malloc(sizeof(TYPE) * new_cap);

    // Set values of temp to data array.
    for (int i = 0, size = da->size; i < size; i++) {
        temp[i] = da->data[i];
    }

    da->data = temp;
    if (temp != 0) {
        free(temp);
        temp = 0;
    }
}

/**
 * Gets the size of the dynamic array.
 */
int dynarr_get_size(struct dynarr_t *da)
{
    return da->size;
}

/**
 * Adds an element to the end of the dynamic array.
 */
void dynarr_append(struct dynarr_t *da, TYPE val)
{
    // Check to see if a resize is necessary.
    if(da->size >= da->capacity) {
        _dynarr_set_cap(da, 2 * da->capacity);
    }
    da->data[da->size] = val;
    da->size++;    

}

/**
 * Gets an element from the dynamic array from a specified position.
 */
TYPE dynarr_get_pos(struct dynarr_t *da, int pos)
{
    // assert(pos < da->size);
    return da->data[pos];
}

/**
 * Gets the data array.
 */
TYPE *dynarr_get_data(struct dynarr_t *da)
{
    return da->data;
}

/**
 * Put an item into the dynamic array at the specified location, overwriting 
 * the element that was there.
 */
void dynarr_put(struct dynarr_t *da, int pos, TYPE val)
{
    assert(pos < da->size);
    da->data[pos] = val;
}

/**
 * Swap two specified elements in the dynamic array.
 */
void dynarr_swap(struct dynarr_t *da, int i, int  j)
{
    TYPE temp = da->data[i];
    da->data[i] = da->data[j];
    da->data[j] = temp;
}

/**
 * Removes the element at the specified location from the array, shifts other
 * elements back one to fill the gap.
 */
void dynarr_remove(struct dynarr_t *da, int idx)
{
    // assert(idx < da->size);
    // Shift all elements at index+ forward.
    for (int i = idx, size = da->size - 1; i < size; i++) {
        da->data[i] = da->data[i + 1];
    }
    da->size--;
}
