#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>

// Define the node structure
typedef struct Node {
    int data;
    struct Node* next;
    struct Node* prev;
} Node;

// Function to create a new node
Node* createNode(int data);

// Insert at the beginning
void insertAtBeginning(Node** head, int data);

// Insert at the end
void insertAtEnd(Node** head, int data);

// Delete a node by value
void deleteNode(Node** head, int value);

// Display the list forward
void displayForward(Node* head);

// Display the list backward
void displayBackward(Node* head);

#endif //LINKEDLIST_H
