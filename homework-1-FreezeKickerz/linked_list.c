//Name: Tyler Korz
//PSU Email: tkk5297@psu.edu    
//PSU ID: 967407712

#include "linked_list.h"

// Function to create a new node
Node* createNode(int data) {
    //TODO: IMPLEMENT THIS
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

// Insert at the beginning
void insertAtBeginning(Node** head, int data) {
    //TODO: IMPLEMENT THIS
    Node* newNode = createNode(data);
    //Check if list is empty
    if(*head == NULL){
        *head = newNode;
        return;
    }
    else{
    //Insert node
    newNode->next = *head;
    (*head)->prev = newNode;
    *head = newNode;
    }
}

// Insert at the end
void insertAtEnd(Node** head, int data) {
    //TODO: IMPLEMENT THIS
    Node* newNode = createNode(data);
    //Check if list is empty
    if(*head == NULL){
        *head = newNode;
        return;
    }
    //Traverse Through nodes untill end
    Node* temp = *head;
    while (temp->next != NULL){
        temp = temp->next;
    }
    //Insert node
    temp->next = newNode;
    newNode->prev = temp;
}

// Delete a node by value
void deleteNode(Node** head, int value) {
    //TODO: IMPLEMENT THIS
    //If list is empty
    if (*head == NULL){
        return;
    }
    Node* temp = *head;   
    //Search for node
    while(temp != NULL && temp->data !=value){
        temp = temp->next;
    }
    //If value is not found
    if (temp == NULL){
        return;
    }
    //Delete Head node
    if (temp == *head){
        *head = temp->next;
        if (*head != NULL) {
            (*head)->prev = NULL;
        }
    } else {
        if (temp->prev != NULL) {
            temp->prev->next = temp->next;
        }
        if (temp->next != NULL) {
            temp->next->prev = temp->prev;
        }
    }
    free(temp);    
}

// Display the list forward
void displayForward(Node* head) {
    //TODO: IMPLEMENT THIS
    Node* temp = head;
    printf("List (Forward): ");
    while (temp != NULL){
        printf("%d ", temp->data);
        temp = temp->next;
    }
    printf("\n");
}

// Display the list backward
void displayBackward(Node* head) {
    //TODO: IMPLEMENT THIS
    if(head ==  NULL){
        return;
    }
    Node* temp = head;
    //Move to end of list
    while(temp->next != NULL){
        temp = temp->next;
    }
    printf("List (Backward): ");
    //Reverse order print
    while(temp != NULL){
        printf("%d ", temp->data);
        temp = temp->prev;
    }
    printf("\n");
}

