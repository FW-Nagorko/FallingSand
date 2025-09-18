#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdlib.h>


/*
 * inspired by https://github.com/eteran/c-vector implementation
 * vector pointer is indistinguishable from a pointer to the first element of the array
 * Vector memory layout:
 *     | vector_metadata_t | vector[0] | vector[1] | ... | vector[n] |
 */

#ifndef VECTOR_DEFAULT_CAPACITY
#define VECTOR_DEFAULT_CAPACITY 8
#endif

typedef struct _vector_metadata {
    size_t size;
    size_t capacity;
} vector_metadata_t;

#define _vector_metadata(vec) ((vec) ? ((vector_metadata_t*)((char*)(vec) - sizeof(vector_metadata_t))) : NULL)

#define Vector_generic void*
#define Vector(type) type*

/// @brief Gets the size of the vector.
/// @param vec The vector.
/// @return The number of elements in the vector.
#define vector_size(vec) ((vec) ? _vector_metadata(vec)->size : 0)

/// @brief Gets the capacity of the vector.
/// @param vec The vector.
/// @return The total allocated capacity of the vector.
#define vector_capacity(vec) ((vec) ? (_vector_metadata(vec))->capacity : 0)

/// @brief Adds an element to the end of the vector.
/// @param vec The vector.
/// @param elem The element to add.
#define vector_push_back(vec, ...) do { \
    if(vec) { \
        if(vector_size(vec) == vector_capacity(vec)) \
            (vec) = _grow((vec), sizeof(*(vec)), _vector_metadata(vec)->capacity * 2); \
    } else { \
        (vec) = _grow((vec), sizeof(*(vec)), VECTOR_DEFAULT_CAPACITY); \
    } \
    (vec)[_vector_metadata(vec)->size++] = (__VA_ARGS__); \
} while(0)


/// @brief Removes the last element from the vector.
/// @param vec The vector.
#define vector_pop_back(vec) do{\
        if(vec) { \
            vector_metadata_t *data = _vector_metadata(vec); \
            if(data->size > 0) { \
                data->size--; \
            } \
        } \
    } while(0)

/// @brief Removes an element from the vector at the specified index.
/// @param vec The vector.
/// @param index The index of the element to remove.
#define vector_erase(vec, index) _vector_erase((vec), sizeof(*(vec)), index)

/// @brief Removes an element from the vector at the specified index without preserving order.
/// @param vec The vector.
/// @param index The index of the element to remove.
#define vector_lazy_erase(vec, index) do { \
    if(vec) { \
        vector_metadata_t *data = _vector_metadata(vec); \
        if(index < data->size) { \
            (vec)[index] = (vec)[data->size - 1]; \
        } \
        vector_pop_back(vec); \
    } \
} while(0)

/// @brief Iterates over the vector.
/// @param vec The vector.
/// @param i The loop variable.
#define for_vector(vec, i) for(size_t i = 0; i < vector_size(vec); i++)

/// @brief Frees the memory allocated for the vector.
/// @param vec The vector.
#define vector_free(vec) (vec) ? free((char*)(vec) - sizeof(vector_metadata_t)) : NULL



void* _grow(void* vec, size_t elemSize, size_t newCapacity);
void _pop_back(void* vec, size_t elemSize);
void _vector_erase(void* vec, size_t elemSize, size_t index);

#endif