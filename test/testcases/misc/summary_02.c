#include <stdio.h>
#include <stdlib.h>

int* safe_function(int *arr, unsigned size, unsigned index) {
    // @assume arr != \nullptr
    // @assume \bound(arr) == size
    if (index >= size) {
        return arr;
    } else {
        return arr + index;
    }
}

int* unsafe_function(int *arr, unsigned size, unsigned index) {
    // @assume arr != \nullptr
    // @assume \bound(arr) == size
    if (index >= size) {
        return NULL;
    } else {
        return arr + index;
    }
}

int main() {
    printf("%i", *safe_function(arr, 5, 3));
    printf("%i", *unsafe_function(arr, 5, 3));
    return 0;
}