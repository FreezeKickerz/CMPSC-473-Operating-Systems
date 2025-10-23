#include "linked_list.h"
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define mu_str_(text) #text
#define mu_str(text) mu_str_(text)
#define mu_assert(message, test) do { if (!(test)) return "See " __FILE__ " Line " mu_str(__LINE__) ": " message; } while (0)

static char* test_insert_at_beginning(Node** head, int data){
    Node* old_head = *head;
    insertAtBeginning(head, data);
    mu_assert("Testing insert at beginning: checking that list is non-empty", *head != NULL);
    mu_assert("Testing insert at beginning: correct data value", (*head)->data == data);
    mu_assert("Testing insert at beginning: correct next node", (*head)->next == old_head);
    return NULL;
}

static char* test_insert_at_end(Node** head, int data){
    Node* old_tail = *head;
    if(old_tail){
        while(old_tail->next){
            old_tail = old_tail->next;
        }
    }

    insertAtEnd(head, data);
    mu_assert("Testing insert at end: checking that list is non-empty", *head != NULL);
    Node* new_node = *head;
    while(new_node->next){
        new_node = new_node->next;
    }
    mu_assert("Testing insert at end: correct data value", new_node->data == data);
    mu_assert("Testing insert at end: correct previous node", new_node->prev == old_tail);
    
    return NULL;
}

static char* test_delete(Node** head, int value){
    deleteNode(head, value);
    if(value == 10){
        mu_assert("Testing delete: checking that list is non-empty", *head != NULL);
        mu_assert("Testing delete: checking node values", (*head)->data == 20);
        mu_assert("Testing delete: checking node values", (*head)->next->data == 30);
        mu_assert("Testing delete: checking node values", (*head)->next->next->data == 40);
    }

    if(value == 50){
        mu_assert("Testing delete: checking that list is non-empty", *head != NULL);
        mu_assert("Testing delete: checking node values", (*head)->data == 20);
        mu_assert("Testing delete: checking node values", (*head)->next->data == 30);
        mu_assert("Testing delete: checking node values", (*head)->next->next->data == 40);
    }

    if(value == 40){
        mu_assert("Testing delete: checking that list is non-empty", *head != NULL);
        mu_assert("Testing delete: checking node values", (*head)->data == 20);
        mu_assert("Testing delete: checking node values", (*head)->next->data == 30);
    }

    if(value == 60){
        mu_assert("Testing delete: checking that list is non-empty", *head != NULL);
        mu_assert("Testing delete: checking node values", (*head)->data == 20);
        mu_assert("Testing delete: checking node values", (*head)->next->data == 30);
    }

    if(value == 30){
        mu_assert("Testing delete: checking that list is non-empty", *head != NULL);
        mu_assert("Testing delete: checking node values", (*head)->data == 20);
    }

    if(value == 70){
        mu_assert("Testing delete: checking that list is non-empty", *head != NULL);
        mu_assert("Testing delete: checking node values", (*head)->data == 20);
    }

    if(value == 20){
        mu_assert("Testing delete: checking that list is empty", *head == NULL);
    }

    return NULL;
}

static char* test_display(Node* head, bool forward, char* out, char* expected){
  int old_stdout, new;

  //redirect stdout to out file
  fflush(stdout);
  old_stdout = dup(1);
  new = open(out, O_CREAT|O_WRONLY|O_TRUNC, 0644);
  dup2(new, 1);
  close(new);

  if(forward){
    displayForward(head);
  }
  else{
    displayBackward(head);
  }
  
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
    printf("%s forwardOutputFile forwardExpectedFile backwardOutputFile backwardExpectedFile\n", program);
}


int main(int argc, char* argv[]) {
    if(argc != 5){
        usage(argv[0]);
        return -1;
    }
    
    char* forwardOut = argv[1];
    char* forwardExpected = argv[2];
    char* backwardOut = argv[3];
    char* backwardExpected = argv[4];

    Node* head = NULL;
    int tests_passed = 0;
    int num_tests = 13;
    char* message1 = test_insert_at_beginning(&head, 20);
    if(message1){
        printf("FAILURE: %s: %s\n", "test_insert_at_beginning", message1);
    }
    else{
        tests_passed += 1;
    }

    char* message2 = test_insert_at_end(&head, 30);
    if(message2){
        printf("FAILURE: %s: %s\n", "test_insert_at_end", message2);
    }
    else{
        tests_passed += 1;
    }
    
    char* message3 = test_insert_at_beginning(&head, 10);
    if(message3){
        printf("FAILURE: %s: %s\n", "test_insert_at_beginning", message3);
    }
    else{
        tests_passed += 1;
    }
    
    char* message4 = test_insert_at_end(&head, 40);
    if(message4){
        printf("FAILURE: %s: %s\n", "test_insert_at_end", message4);
    }
    else{
        tests_passed += 1;
    }

    char* message12 = test_display(head, true, forwardOut, forwardExpected);
    if(message12){
        printf("FAILURE: %s: %s\n", "test_display", message12);
    }
    else{
        tests_passed += 1;
    }
    
    char* message13 = test_display(head, false, backwardOut, backwardExpected);
    if(message13){
        printf("FAILURE: %s: %s\n", "test_display", message13);
    }
    else{
        tests_passed += 1;
    }
    
    char* message5 = test_delete(&head, 10);
    if(message5){
        printf("FAILURE: %s: %s\n", "test_delete", message5);
    }
    else{
        tests_passed += 1;
    }
    
    char* message6 = test_delete(&head, 50);
    if(message6){
        printf("FAILURE: %s: %s\n", "test_delete", message6);
    }
    else{
        tests_passed += 1;
    }
    
    char* message7 = test_delete(&head, 40);
    if(message7){
        printf("FAILURE: %s: %s\n", "test_delete", message7);
    }
    else{
        tests_passed += 1;
    }
    
    char* message8 = test_delete(&head, 60);
    if(message8){
        printf("FAILURE: %s: %s\n", "test_delete", message8);
    }
    else{
        tests_passed += 1;
    }
    
    char* message9 = test_delete(&head, 30);
    if(message9){
        printf("FAILURE: %s: %s\n", "test_delete", message9);
    }
    else{
        tests_passed += 1;
    }
    
    char* message10 = test_delete(&head, 70);
    if(message10){
        printf("FAILURE: %s: %s\n", "test_delete", message10);
    }
    else{
        tests_passed += 1;
    }
    
    char* message11 = test_delete(&head, 20);
    if(message11){
        printf("FAILURE: %s: %s\n", "test_delete", message11);
    }
    else{
        tests_passed += 1;
    }

    printf("Linked List Test Cases Passed: %d/%d\n", tests_passed, num_tests);
    if(tests_passed != num_tests){
        return 1;
    }
    return 0;
    
}

