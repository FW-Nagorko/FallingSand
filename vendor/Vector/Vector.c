#include "Vector.h"
#include <stdlib.h>
#include <string.h>

void* _grow(void *vec, size_t elemSize, size_t newCapacity) {
    vector_metadata_t *data;
    if(vec) {
        data = _vector_metadata(vec);
        data->capacity = newCapacity;
        size_t newSize = sizeof(vector_metadata_t) + data->capacity * elemSize;
        data = realloc(data, newSize);
        vec = (char *)data + sizeof(vector_metadata_t);
    }else{
        data = malloc(sizeof(vector_metadata_t) + elemSize * newCapacity);
        data->size = 0;
        data->capacity = newCapacity;
        vec = (char *)data + sizeof(vector_metadata_t);
    }
    return vec;
}


void _vector_erase(void *vec, size_t elemSize, size_t index) {
    if(vec) {
        vector_metadata_t *data = _vector_metadata(vec);
        if(index < data->size) {
            size_t numberOfBytes = (data->size - index - 1) * elemSize;
            memmove((char *)vec + index * elemSize, (char *)vec + (index + 1) * elemSize, numberOfBytes);
            data->size--;
        }
    }
}
