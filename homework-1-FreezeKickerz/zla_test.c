#include "zla.h"
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define mu_str_(text) #text
#define mu_str(text) mu_str_(text)
#define mu_assert(message, test) do { if (!(test)) return "See " __FILE__ " Line " mu_str(__LINE__) ": " message; } while (0)


static char* test_create(int num_elements, ZeroLengthArray** arr_holder)
{
    ZeroLengthArray* arr = create_zero_length_array(num_elements); 
    mu_assert("Testing array allocation was successful", arr != NULL);

    *arr_holder = arr;

    mu_assert("Testing size field is set correctly", arr->size == num_elements);

    return NULL;
}


static char* test_set(ZeroLengthArray* arr, int* values)
{
    set_values(arr, values);
    mu_assert("Testing array is not NULL after setting values", arr != NULL);
    for(int i = 0; i < arr->size; i++){
        mu_assert("Testing array element is set correctly", arr->data[i] == values[i]);
    }
    
    return NULL;
}

static char* test_print(ZeroLengthArray* arr, char* out, char* expected){
    int old_stdout, new;

    //redirect stdout to out file
    fflush(stdout);
    old_stdout = dup(1);
    new = open(out, O_CREAT|O_WRONLY|O_TRUNC, 0644);
    dup2(new, 1);
    close(new);
    
    print_zero_length_array(arr);
  
    fflush(stdout);
    dup2(old_stdout, 1);
    close(old_stdout);

    //run diff on out file and expected
    size_t len = strlen(out) + strlen(expected) + strlen("diff  ") + 1;
    char* command = malloc(len);
    snprintf(command, len, "diff %s %s", out, expected);
    int result = system(command);
    
    mu_assert("Testing display: checking that output is as expected", result == 0);
    free(command);
    return NULL;
}


void usage(char* program)
{
    printf("Please run the program as follows: \n");
    printf("%s outputFile expectedFile\n", program);
}

int main(int argc, char* argv[]) {
    if(argc != 3){
        usage(argv[0]);
        return -1;
    }
    
    char* out = argv[1];
    char* expected = argv[2];

    // Create a zero-length array with the specified number of elements
    ZeroLengthArray* arr1 = NULL;
    int tests_passed = 0;
    int num_tests = 11;
    
    char* message1 = test_create(1, &arr1);
    if(message1){
        printf("FAILURE: %s: %s\n", "test_create", message1);
    }
    else{
        tests_passed += 1;
    }
    
    ZeroLengthArray* arr2 = NULL;
    char* message2 = test_create(2, &arr2);
    if(message2){
        printf("FAILURE: %s: %s\n", "test_create", message2);
    }
    else{
        tests_passed += 1;
    }
    
    ZeroLengthArray* arr3 = NULL;
    char* message3 = test_create(3, &arr3);
    if(message3){
        printf("FAILURE: %s: %s\n", "test_create", message3);
    }
    else{
        tests_passed += 1;
    }
    
    ZeroLengthArray* arr4 = NULL;
    char* message4 = test_create(4, &arr4);
    if(message4){
        printf("FAILURE: %s: %s\n", "test_create", message4);
    }
    else{
        tests_passed += 1;
    }
    
    ZeroLengthArray* arr5 = NULL;
    char* message5 = test_create(5, &arr5);
    if(message5){
        printf("FAILURE: %s: %s\n", "test_create", message5);
    }
    else{
        tests_passed += 1;
    }
    
    // Define some values to put in the array
    int values1[] = {1};
    int values2[] = {1, 2};
    int values3[] = {1, 2, 3};
    int values4[] = {1, 2, 3, 4};
    int values5[] = {1, 2, 3, 4, 5};
    
    // Set the values in the array
    char* set_message1 = test_set(arr1, values1);
    if(set_message1){
        printf("FAILURE: %s: %s\n", "test_set", set_message1);
    }
    else{
        tests_passed += 1;
    }

    char* set_message2 = test_set(arr2, values2);
    if(set_message2){
        printf("FAILURE: %s: %s\n", "test_set", set_message2);
    }
    else{
        tests_passed += 1;
    }
    
    char* set_message3 = test_set(arr3, values3);
    if(set_message3){
        printf("FAILURE: %s: %s\n", "test_set", set_message3);
    }
    else{
        tests_passed += 1;
    }

    char* set_message4 = test_set(arr4, values4);
    if(set_message4){
        printf("FAILURE: %s: %s\n", "test_set", set_message4);
    }
    else{
        tests_passed += 1;
    }
    
    char* set_message5 = test_set(arr5, values5);
    if(set_message5){
        printf("FAILURE: %s: %s\n", "test_set", set_message5);
    }
    else{
        tests_passed += 1;
    }

    char* print_message1 = test_print(arr5, out, expected);
    if(print_message1){
        printf("FAILURE: %s: %s\n", "test_print", print_message1);
    }
    else{
        tests_passed += 1;
    }
    
    // Don't forget to free the allocated memory
    free(arr1);
    free(arr2);
    free(arr3);
    free(arr4);
    free(arr5);

    printf("Zero-Length Array Test Cases Passed: %d/%d\n", tests_passed, num_tests);
    return 0;
}
