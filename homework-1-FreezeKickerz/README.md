[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/wwzOfJnh)
### Please include your name, email, and PSU ID (9 digit) where intidacted in the following files:
`linked_list.c`
`polymorphic_ints.c`
`zla.c`
`kobject.c`

**This assignment is worth 5% of your course grade.**

### Homework Question 1: Implementing a Doubly Linked List in C

**Question**:  
Write a C program to implement a **doubly linked list** with the following functionalities:

1. **Insert at the Beginning**: Add a new node at the start of the list.  
2. **Insert at the End**: Add a new node at the end of the list.  
3. **Delete a Node by Value**: Remove the first occurrence of a node with a specified value.  
4. **Display Forward**: Print the elements of the list from the head to the tail.  
5. **Display Backward**: Print the elements of the list from the tail to the head.  

### Requirements:
- Create a `struct` for the doubly linked list node, which should contain:
  - An integer `data` field to store the value.
  - Two pointers, `next` and `prev`, to point to the next and previous nodes, respectively.
- Dynamically allocate memory for each new node.
- Handle edge cases, such as:
  - Inserting into an empty list.
  - Deleting from an empty list.
  - Attempting to delete a value that does not exist in the list.
- Ensure memory is properly managed to avoid memory leaks (e.g., free nodes after deletion).
- Please see the expected output from `displayForward` and `displayBackward` in the `linked_list_output/forward_expected` and `linked_list_output/backward_expected` files, respectively.  Please follow this format to ensure that the test cases pass.

**You can run your code in an interactive way by using `make interactive` and `make run`.  An example of interactive output is below.**
```plaintext
Choose an option:
1. Insert at the Beginning
2. Insert at the End
3. Delete a Node by Value
4. Display Forward
5. Display Backward
6. Exit
Enter your choice: 1
Enter the value to insert: 10

Choose an option:
1. Insert at the Beginning
2. Insert at the End
3. Delete a Node by Value
4. Display Forward
5. Display Backward
6. Exit
Enter your choice: 2
Enter the value to insert: 20

Choose an option:
1. Insert at the Beginning
2. Insert at the End
3. Delete a Node by Value
4. Display Forward
5. Display Backward
6. Exit
Enter your choice: 4
List (Forward): 10 20

Choose an option:
1. Insert at the Beginning
2. Insert at the End
3. Delete a Node by Value
4. Display Forward
5. Display Backward
6. Exit
Enter your choice: 3
Enter the value to delete: 10

Choose an option:
1. Insert at the Beginning
2. Insert at the End
3. Delete a Node by Value
4. Display Forward
5. Display Backward
6. Exit
Enter your choice: 4
List (Forward): 20
```
### Grading Criteria:
- **Correctness (50%)**: All functions should work as intended, and the output should match the expected results.
- **Dynamic Memory Allocation (25%)**: Memory should be allocated and freed correctly.
- **Code Readability (25%)**: The code should be well-organized and include meaningful comments.


Good luck!

### Homework Question 2: Working with Tagged Unions in C

**Objective**:  
This assignment aims to help students understand and implement **tagged unions** in C. Tagged unions are a useful way to store and interpret data of different types in a single structure. Students will work with a provided tagged union structure to implement and extend a function that processes data stored in these unions.

---

### Task:  
You are required to use the `TaggedUnion` structure provided below and implement the `addTaggedUnions` function. This function takes an array of `TaggedUnion` objects and computes the sum of their values based on their type.

#### TaggedUnion Structure:
```c
typedef struct {
    enum { INT, CHAR4 } tag; // Tag to indicate the type of data
    union {
        int asInt;               // 32-bit integer
        unsigned char asChar[4]; // Array of 4 8-bit numbers
    } data;
} TaggedUnion;
```

---

### Requirements:
1. **Understand the Structure**:
   - The `tag` field indicates the type of data stored in the union:
     - `INT`: The `asInt` field contains a 32-bit integer.
     - `CHAR4`: The `asChar` array contains 4 unsigned 8-bit numbers.
   - The `data` field stores the actual value, either as an integer or an array of characters.

2. **Implement the Function**:
   - Write the `addTaggedUnions` function with the following signature:
     ```c
     int addTaggedUnions(TaggedUnion *unions, size_t count);
     ```
   - The function should:
     - Iterate through the array of `TaggedUnion` objects.
     - Add the integer value if the `tag` is `INT`.
     - Add the sum of the 4 characters if the `tag` is `CHAR4`.
   - Return the total sum.

3. **Edge Cases**:
   - Handle an empty array of `TaggedUnion` objects.
   - Ensure the function works for large arrays with mixed types.

---

### Example Program:
Here is an example program to guide your implementation:

#### Input:
```c
TaggedUnion unions[3];

// Initialize as integer
unions[0].tag = INT;
unions[0].data.asInt = 42;

// Initialize as 4 characters
unions[1].tag = CHAR4;
unions[1].data.asChar[0] = 10;
unions[1].data.asChar[1] = 20;
unions[1].data.asChar[2] = 30;
unions[1].data.asChar[3] = 40;

// Initialize as another integer
unions[2].tag = INT;
unions[2].data.asInt = 100;

// Call addTaggedUnions
int result = addTaggedUnions(unions, 3);
printf("Sum: %d\n", result);
```

#### Output:
```plaintext
Sum: 242
```

---

### Grading Criteria:
- **Correctness (75%)**: All functions should work as intended, and the output should match the expected results.
- **Code Readability (25%)**: The code should be well-organized and include meaningful comments.

Good luck!

### Homework Question 3: Working with Zero-Length Arrays in C

**Objective**:
This assignment introduces students to **zero-length arrays**, a flexible feature in C used to create dynamically sized arrays as part of a structure. Students will use the provided `ZeroLengthArray` structure to implement and manipulate a dynamic array.

---

### Task:
You are provided with a structure containing a zero-length array and a main function that demonstrates its usage. Your task is to use the `ZeroLengthArray` structure to dynamically allocate memory for an array, set values, and perform operations on it.

#### ZeroLengthArray Structure:
```c
typedef struct {
    int size;      // Number of elements in the array
    int data[];    // Flexible array member (zero-length array)
} ZeroLengthArray;
```

---

### Requirements:
1. **Understand the Structure**:
   - The `size` field stores the number of elements in the array.
   - The `data` field is a zero-length array, allowing the structure to be dynamically extended to hold any number of integers.

2. **Implement the Following Functions**:
   - **`create_zero_length_array`**:
     - Dynamically allocate memory for the `ZeroLengthArray` structure and its array.
     - Store the number of elements in the `size` field.
     - Return a pointer to the allocated structure.
   - **`set_values`**:
     - Assign values to the array using an input array.
   - **`print_zero_length_array`**:
     - Print the elements of the array, along with its size.
     - Please follow the exact format seen in the `zla_output/out_expected` file.

3. **Memory Management**:
   - Properly allocate memory for the structure and its array.
   - Free the allocated memory after usage to prevent memory leaks.

---

### Example Program Behavior:
#### Input:
The program dynamically creates a `ZeroLengthArray` with 5 elements and sets their values to `{1, 2, 3, 4, 5}`.

#### Output:
```plaintext
List elements (size = 5):
1 2 3 4 5
```

---

### Grading Criteria:
- **Correctness (50%)**: All functions should work as intended, and the output should match the expected results.
- **Dynamic Memory Allocation (25%)**: Memory should be allocated and freed correctly.
- **Code Readability (25%)**: The code should be well-organized and include meaningful comments.

---

### Notes:
- Zero-length arrays are a flexible way to create variable-length data structures.
- Always ensure proper memory management to avoid leaks or undefined behavior.

Good luck!


### Homework Question 4: Establishing and Printing Device Hierarchies Using kobjects and ksets

**Objective**:
This assignment focuses on simulating a device and bus hierarchy using **kobjects** and **ksets**. Students will implement and manage relationships between devices, buses, and bridges, and print the resulting hierarchy.

---

### Task:
You are provided with a partially implemented program that initializes and organizes devices and buses using `kobjects` and `ksets`. Your goal is to complete the implementation to achieve the functionality demonstrated in the provided output example. refer to the `bus_topology.pdf` file for a visual example.

---

### Requirements:

1. **Understand the Program Structure**:
   - **`kobject`**: Represents individual entities (devices, buses, bridges) with a name, parent, and reference count.
   - **`kset`**: Represents a collection of `kobjects` (e.g., devices on a bus).
   - The structures `kobject` and `kset` are already provided in `kobject.h` file.
   - `kobject_init()`, `kset_init()`  and `kset_add()` functions are already provided in `kobject.c` file. These can be used while implementing the TODOs.

2. **Implement the Following Functions**:
   - **`device_init` **:
     - Initialize a `Device` object with a name and associate it with a parent `Bus` or `Bridge`.
   - **`bridge_init` **:
     - Initialize a `Bridge` object with a name and associate it with a parent `Bus`.
   - **`bus_init` **:
     - Initialize a `Bus` object with a name.
   - **`bus_add_device`**:
     - Add a `Device` to a `Bus` by updating the `kset` of the `Bus`.
   - **`print_hierarchy`**:
     - Recursively print the hierarchy of `kobjects`, starting from the root.
     - Indent child objects to visually represent their parent-child relationships.
   - **`bus_print_hierarchy`**:
     - Print the hierarchy of a specific bus, starting from its root `kobject`.

3. **Test the Implementation**:
   - Use the `main` function in `kobject_test.c` file to test the implementation.
   - Initialize multiple buses, devices, and bridges for the given topology in `bus_topology.pdf`.
   - Establish relationships between buses, devices, and bridges for the given topology in `bus_topology.pdf`.
   - Print the hierarchy of each bus to ensure the relationships are displayed correctly. Compare with the output in `traces/expected_output` to ensure that your implementation passes the test.

---

### Example Program Behavior:

#### Output:
```plaintext
Initialized Bus 'BusA'.
Initialized Device 'Device_a'.
Initialized Device 'Device_b'.
Initialized Bus 'BusB'.
Initialized Device 'BusA2BusB'.
Added kobject 'BusA2BusB' to kset 'BusA'.
Initialized Device 'Device_c'.
Added kobject 'Device_a' to kset 'BusA'.
Added kobject 'Device_b' to kset 'BusA'.
Added kobject 'Device_c' to kset 'BusB'.
Hierarchy of Bus 'BusA':
kobject: BusA
  kobject: BusA2BusB
    kobject: BusA
  kobject: Device_a
    kobject: BusA
  kobject: Device_b
    kobject: BusA
Hierarchy of Bus 'BusB':
kobject: BusB
  kobject: BusA2BusB
    kobject: BusA
  kobject: Device_c
    kobject: BusB
      kobject: BusA2BusB
        kobject: BusA
```

#### Explanation of Output of Print function:
```plaintext
Hierarchy of Bus 'BusA':
kobject: BusA             \\This is the root
  kobject: BusA2BusB      \\BusA2BusB is a member of kset of BusA
    kobject: BusA         \\BusA is parent of BusA2BusB
  kobject: Device_a       \\Device_a is a member of kset of BusA
    kobject: BusA         \\BusA is parent of Device_a
  kobject: Device_b       \\Device_b is a member of kset of BusA
    kobject: BusA         \\BusA is parent of Device_b
Hierarchy of Bus 'BusB':
kobject: BusA2BusB        \\This is the root
  kobject: BusA2BusB      \\BusA2BusB is a member of kset of BusB
    kobject: BusA         \\BusA is parent of BusA2BusB
  kobject: Device_c       \\Device_c is a member of kset of BusB
    kobject: BusB         \\BusB is parent of Device_c
      kobject: BusA2BusB  \\BusA2BusB is parent of BusB
        kobject: BusA     \\BusA is parent of BusA2BusB
```

---

### Instructions:

1. **Initialization**:
   - Use the provided functions to initialize buses, devices, and bridges.
   - Ensure all `kobjects` are properly initialized with names and parent relationships.
   - You are allowed to add members to structs `Device`, `Bus` and `Bridge`.
   - These changes should be done in `kobject.h` and `kobject.c` files. Do not delete any part of the provided code.

2. **Hierarchy Management**:
   - Make use of `kset_add` to add devices and bridges to their respective buses.
   - Ensure each object is correctly associated with its parent.
   - These changes should be done in the `kobject_test.c` file. Do not delete any part of the provided code.

3. **Printing the Hierarchy**:
   - Implement recursive printing to display the hierarchy of each bus.
   - Indent child objects for clarity.
   - These changes should be done in the `kobject.c` file.
---

### Submission Requirements:
1. Make edits in the `kobject.h`, `kobject.c` and `kobject_test.c` files.
2. Include comments explaining the functionality of each function.
3. Do not delete any part of the provided code.
4. Do not change any of the function definitions.
5. Do not hardcode variable names in prints and do not hardcode the `print_hierarchy()` and the `bus_print_hierarchy()` functions.
6. Output should match exactly the provided `traces/expected_output` file.
---

### Grading Criteria:
- **Correctness and Hierarchy Representation (90%)**: The program should correctly initialize objects, establish relationships, and print the hierarchy. The output should match the example provided, with proper indentation for parent-child relationships.
- **Code Readability (10%)**: The code should be well-organized and include meaningful comments.

---

### Notes:
- This assignment simulates a kernel-like hierarchy management system using `kobjects` and `ksets`.
- Pay special attention to the parent-child relationships and ensure the hierarchy is correctly maintained.

Good luck!

---
### Submission Requirements (for all questions):
1. Add/commit/push your changes to this repository. Submit the commit id you would like to be graded to Canvas. 
2. Include comments explaining the functionality of each function in each question.
3. Ensure your program compiles without errors using `gcc` or a similar compiler.

---
### Running your code
To run all of the provided test cases, use `make` and `make test`.  This will show you how many test cases passed for each question.
You can also run each program (i.e., for each question) individually.  This is a bit different for each question:
1. Question 1: `./linked_list linked_list_output/forward linked_list_output/forward_expected linked_list_output/backward linked_list_output/backward_expected`
2. Question 2: `./polymorphic_ints`
3. Question 3: `./zla zla_output/out zla_output/out_expected`
4. Question 4: `./kobject`
