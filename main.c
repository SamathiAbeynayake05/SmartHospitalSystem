#include <stdio.h>
#include "hospital.h"

int main(void) {
    int choice;

    printf("Smart Hospital Patient & Resource Allocation System\n");

    do {
        showMainMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                registerPatient();
                break;
            case 2:
                printf("\n[View Bed Occupancy - not implemented yet]\n");
                break;
            case 3:
                printf("\n[View Sorted Patients - not implemented yet]\n");
                break;
            case 4:
                printf("\n[Generate Reports - not implemented yet]\n");
                break;
            case 5:
                printf("\nGoodbye!\n");
                break;
            default:
                printf("\nInvalid choice. Please select 1-5.\n");
        }
    } while (choice != 5);

    return 0;
}
