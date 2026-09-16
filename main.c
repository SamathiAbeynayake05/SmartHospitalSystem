#include <stdio.h>
#include "hospital.h"

int main(void) {
    int choice;

    printf("Smart Hospital Patient & Resource Allocation System\n");
    loadBedStatus();

    do {
        showMainMenu();
        /* getValidatedInt also clears the input buffer for us, so no
           leftover newline is left behind for the next fgets/scanf call */
        choice = getValidatedInt("Enter your choice (1-5): ", 1, 5);

        switch (choice) {
            case 1:
                registerPatient();
                break;
            case 2:
                viewBedOccupancy();
                break;
            case 3:
                viewSortedPatients();
                break;
            case 4:
                generateReports();
                break;
            case 5:
                saveBedStatus();
                printf("\nBed status saved. Goodbye!\n");
                break;
        }
    } while (choice != 5);

    return 0;
}
