//Name: Tyler Korz
//PSU Email: tkk5297@psu.edu    
//PSU ID: 967407712

#include "zla.h"

// Function to create a ZeroLengthArray
ZeroLengthArray* create_zero_length_array(int num_elements) {
    //TODO: IMPLEMENT THIS
    ZeroLengthArray* arr = malloc(sizeof(ZeroLengthArray) + num_elements * sizeof(int));
    arr->size = num_elements;
    return arr;
}

// Function to print the list
void print_zero_length_array(ZeroLengthArray* arr) {
    //TODO: IMPLEMENT THIS
    printf("List elements (size = %d):\n", arr->size);
    for(int j=0; j < arr->size; j++){
        printf("%d ", arr->data[j]);
    }
    printf("\n");
}

// Function to set values in the array
void set_values(ZeroLengthArray* arr, int* values) {
    //TODO: IMPLEMENT THIS
    for(int j=0; j < arr->size; j++){
        arr->data[j] = values[j];
    }
}

