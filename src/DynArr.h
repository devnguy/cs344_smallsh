#ifndef DYN_ARR_H
#define DYN_ARR_H

#ifndef __TYPE
#define __TYPE
#define TYPE int
#define TYPE_SIZE sizeof(int)
#endif

typedef struct dynarr_t DynArr;
void dynarr_init(DynArr *da, int capacity);
DynArr *dynarr_create(int cap);
void dynarr_free(DynArr *da);
void dynarr_destroy(DynArr *da);
void _dynarr_set_cap(DynArr *da, int new_cap);
int dynarr_get_size(DynArr *da);
TYPE *dynarr_get_data(DynArr *da);
void dynarr_append(DynArr *da, TYPE val);
TYPE dynarr_get_pos(DynArr *da, int pos);
void dynarr_put(DynArr *da, int pos, TYPE val);
void dynarr_swap(DynArr *da, int i, int  j);
void dynarr_remove(DynArr *da, int idx);

#endif