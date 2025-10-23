#include "linked_list.h"
// Main function
int main() {
    //Interactive Testing
    Node* head = NULL;
    int choice, value, success;
    while (1) {
        printf("\nChoose an option:\n");
        printf("1. Insert at the Beginning\n");
        printf("2. Insert at the End\n");
        printf("3. Delete a Node by Value\n");
        printf("4. Display Forward\n");
        printf("5. Display Backward\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        success = scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter the value to insert: ");
                success = scanf("%d", &value);
		if(success)
		  insertAtBeginning(&head, value);
                break;
            case 2:
                printf("Enter the value to insert: ");
                success = scanf("%d", &value);
		if(success)
		  insertAtEnd(&head, value);
                break;
            case 3:
                printf("Enter the value to delete: ");
                success = scanf("%d", &value);
		if(success)
		  deleteNode(&head, value);
                break;
            case 4:
                displayForward(head);
                break;
            case 5:
                displayBackward(head);
                break;
            case 6:
                printf("Exiting program.\n");
                // Free memory before exiting
                while (head != NULL) {
                    Node* temp = head;
                    head = head->next;
                    free(temp);
                }
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

