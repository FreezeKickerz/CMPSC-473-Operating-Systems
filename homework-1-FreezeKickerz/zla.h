#include <stdio.h>
#include <stdlib.h>

// Define a struct with a flexible array member
typedef struct {
    int size;      // Number of elements in the array
    int data[];    // Flexible array member (zero-length array)
} ZeroLengthArray;

// Function to create a ZeroLengthArray
ZeroLengthArray* create_zero_length_array(int num_elements);

// Function to print the list
void print_zero_length_array(ZeroLengthArray* arr);

// Function to set values in the array
void set_values(ZeroLengthArray* arr, int* values);
