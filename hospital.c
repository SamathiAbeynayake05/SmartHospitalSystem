#include <stdio.h>
#include <string.h>
#include "hospital.h"

//LOOKUP DATA DEFINITIONS
const char specialtyName[NUM_SPECIALTIES][30] = {
    "General Practice (OPD)", "Paediatrics", "Cardiology", "Neurology"
};
const double specialtyBaseFee[NUM_SPECIALTIES]   = {1500.00, 2500.00, 4500.00, 5000.00};
const int    specialtyConsultTime[NUM_SPECIALTIES] = {15, 20, 30, 30};
const int    specialtyDailyCap[NUM_SPECIALTIES]    = {30, 20, 12, 10};

const char wardName[NUM_WARDS][30] = {
    "General Ward", "Paediatric Ward", "Surgical Ward", "ICU (Intensive Care Unit)"
};
const double wardDailyRate[NUM_WARDS] = {3000.00, 6000.00, 12000.00, 25000.00};
const int    wardCapacity[NUM_WARDS]  = {20, 10, 10, 5};

int bedOccupancy[NUM_WARDS][MAX_BEDS_PER_WARD];
int specialtyQueueCount[NUM_SPECIALTIES] = {0, 0, 0, 0};

int nextPatientId = 1001; /* default starting ID if no next_id.txt exists yet */

/*  PATIENT ARRAYS  */
int    patientID[MAX_PATIENTS];
char   patientName[MAX_PATIENTS][NAME_LEN];
int    patientAge[MAX_PATIENTS];
int    patientUrgency[MAX_PATIENTS];
int    patientSpecialtyIdx[MAX_PATIENTS];
int    patientIsAdmitted[MAX_PATIENTS];
int    patientWardIdx[MAX_PATIENTS];
int    patientBedNumber[MAX_PATIENTS];
int    patientDaysAdmitted[MAX_PATIENTS];

double patientBaseFee[MAX_PATIENTS];
double patientSurcharge[MAX_PATIENTS];
double patientWardCost[MAX_PATIENTS];
double patientGrossTotal[MAX_PATIENTS];
double patientDiscount[MAX_PATIENTS];
double patientFinalAmount[MAX_PATIENTS];
double patientWaitTime[MAX_PATIENTS];

int patientCount = 0;

/*  INPUT VALIDATION
   Keeps re-prompting until the user types a whole number within [min, max].
   Handles two failure cases:
   1. Non-numeric input (e.g. "h") - scanf fails and leaves it in the buffer.
   2. Numeric but out-of-range input (e.g. 9 when only 1-4 are valid).
   Either way, it clears the rest of the input line before retrying, so a
   bad character never leaks into the next read (this is what was crashing
   the program before). */
int getValidatedInt(const char *prompt, int min, int max) {
    int value;
    int result;
    int c;

    while (1) {
        printf("%s", prompt);
        result = scanf("%d", &value);

        /* Clear everything left on this input line, valid or not - this
           is what protects the NEXT scanf/fgets from leftover garbage */
        while ((c = getchar()) != '\n' && c != EOF)
            ;

        if (result == 1 && value >= min && value <= max) {
            return value;
        }
        printf("Invalid input. Please enter a whole number between %d and %d.\n", min, max);
    }
}

void showMainMenu(void) {
    printf("\n============ SMART HOSPITAL SYSTEM ============\n");
    printf("1. Register New Patient\n");
    printf("2. View Bed Occupancy Matrix\n");
    printf("3. View Patients by Priority (Triage Sort)\n");
    printf("4. Generate Performance Reports\n");
    printf("5. Save & Exit\n");
    printf("=================================================\n");
}

/* Finds first free bed in the given ward, marks it occupied, returns its index (0-based).
   Returns -1 if the ward is full. */
int assignBed(int wardIdx) {
    int b;
    for (b = 0; b < wardCapacity[wardIdx]; b++) {
        if (bedOccupancy[wardIdx][b] == 0) {
            bedOccupancy[wardIdx][b] = 1;
            return b;
        }
    }
    return -1;
}

/* Requirement 3.1: Wait Time = current queue count for this specialty * avg time per patient.
   Uses the count BEFORE this patient is added, so the first patient always waits 0 mins. */
double calculateWaitTime(int specialtyIdx) {
    return specialtyQueueCount[specialtyIdx] * specialtyConsultTime[specialtyIdx];
}

/* Requirement 3.2: Emergency Surcharge based on urgency level */
double calculateSurcharge(int urgencyLevel, double baseFee) {
    if (urgencyLevel == 2) return baseFee * 0.20;
    if (urgencyLevel == 3) return baseFee * 0.50;
    return 0.0;
}

/* Requirement 3.3: Total Ward Stay Cost = Days Admitted * Ward Daily Rate (0 if not admitted) */
double calculateWardCost(int daysAdmitted, int wardIdx) {
    if (wardIdx < 0) return 0.0;
    return daysAdmitted * wardDailyRate[wardIdx];
}

/* Requirement 3.4: Gross Total = Base Fee + Surcharge + Ward Cost */
double calculateGrossTotal(double baseFee, double surcharge, double wardCost) {
    return baseFee + surcharge + wardCost;
}

/* Requirement 3.5: Age Subsidy Discount - 15% if age < 5 or age > 65 */
double calculateDiscount(int age, double grossTotal) {
    if (age < 5 || age > 65) return grossTotal * 0.15;
    return 0.0;
}

/* Requirement 3.6: Final Payable Amount = Gross Total - Discount */
double calculateFinalAmount(double grossTotal, double discount) {
    return grossTotal - discount;
}

const char* urgencyLabel(int level) {
    if (level == 3) return "Level 3 (Critical)";
    if (level == 2) return "Level 2 (Urgent)";
    return "Level 1 (Normal)";
}

/* Requirement 5: formatted bill printout */
void displayBill(int idx) {
    printf("====================================================\n");
    printf(" SMART HOSPITAL ADMISSION & BILL\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("Patient ID      : PAT-%d\n", patientID[idx]);
    printf("Patient Name    : %s\n", patientName[idx]);
    if (patientAge[idx] < 5 || patientAge[idx] > 65)
        printf("Age             : %d Years (15%% Subsidy Eligible)\n", patientAge[idx]);
    else
        printf("Age             : %d Years\n", patientAge[idx]);
    printf("Specialty       : %s\n", specialtyName[patientSpecialtyIdx[idx]]);
    if (patientIsAdmitted[idx])
        printf("Assigned Ward   : %s (Bed #%02d)\n", wardName[patientWardIdx[idx]],
               patientBedNumber[idx] + 1);
    else
        printf("Assigned Ward   : Outpatient (Not Admitted)\n");
    printf("Urgency Level   : %s\n", urgencyLabel(patientUrgency[idx]));
    printf("----------------------------------------------------------------------------------------\n");
    printf("Base Consultation Fee : LKR %.2f\n", patientBaseFee[idx]);
    if (patientUrgency[idx] == 2)
        printf("Emergency Surcharge   : LKR %.2f (20%%)\n", patientSurcharge[idx]);
    else if (patientUrgency[idx] == 3)
        printf("Emergency Surcharge   : LKR %.2f (50%%)\n", patientSurcharge[idx]);
    else
        printf("Emergency Surcharge   : LKR %.2f\n", patientSurcharge[idx]);
    if (patientIsAdmitted[idx])
        printf("Ward Stay Cost (%d Days) : LKR %.2f\n", patientDaysAdmitted[idx], patientWardCost[idx]);
    else
        printf("Ward Stay Cost        : LKR 0.00\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("Gross Total Bill      : LKR %.2f\n", patientGrossTotal[idx]);
    if (patientDiscount[idx] > 0)
        printf("Age Subsidy Discount  : LKR -%.2f (15%%)\n", patientDiscount[idx]);
    else
        printf("Age Subsidy Discount  : LKR 0.00\n");
    printf("----------------------------------------------------------------------------------------\n");
    printf("Final Payable Amount  : LKR %.2f\n", patientFinalAmount[idx]);
    if (patientWaitTime[idx] == 0)
        printf("Estimated Waiting Time: 0.00 mins (Immediate Attention)\n");
    else
        printf("Estimated Waiting Time: %.2f mins\n", patientWaitTime[idx]);
    printf("====================================================\n\n");
}

/* Requirement 1: display the bed occupancy matrix */
void viewBedOccupancy(void) {
    int w, b, occupied;

    printf("\n============ BED OCCUPANCY MATRIX ============\n");
    for (w = 0; w < NUM_WARDS; w++) {
        occupied = 0;
        printf("\n%s (Capacity: %d)\n  ", wardName[w], wardCapacity[w]);
        for (b = 0; b < wardCapacity[w]; b++) {
            printf("[%d]", bedOccupancy[w][b]);
            if (bedOccupancy[w][b] == 1) occupied++;
        }
        printf("\n  Occupied: %d / %d (%.1f%%)\n", occupied, wardCapacity[w],
               (occupied * 100.0) / wardCapacity[w]);
    }
    printf("================================================\n");
}

/* Requirement 4: priority sort - fills an index array 'order' rather than
   moving the patient arrays themselves, so registration order stays intact
   as a natural tie-breaker. Bubble sort is stable, so equal urgency levels
   keep their original registration order automatically. */
void sortPatientsByPriority(int order[]) {
    int i, j, temp;

    for (i = 0; i < patientCount; i++) order[i] = i; /* start in registration order */

    for (i = 0; i < patientCount - 1; i++) {
        for (j = 0; j < patientCount - 1 - i; j++) {
            if (patientUrgency[order[j]] < patientUrgency[order[j + 1]]) {
                temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }
}

/* Displays patients ordered by the sort above: Level 3 (Critical) first,
   then Level 2, then Level 1 - registration order preserved within each level */
void viewSortedPatients(void) {
    int order[MAX_PATIENTS];
    int i;

    if (patientCount == 0) {
        printf("\nNo patients registered yet.\n");
        return;
    }

    sortPatientsByPriority(order);

    printf("\n============ PATIENTS BY PRIORITY ============\n");
    printf("%-10s %-20s %-18s %-15s\n", "Pat ID", "Name", "Urgency", "Final Bill (LKR)");
    for (i = 0; i < patientCount; i++) {
        int idx = order[i];
        printf("PAT-%-6d %-20s %-18s %.2f\n",
               patientID[idx], patientName[idx], urgencyLabel(patientUrgency[idx]),
               patientFinalAmount[idx]);
    }
    printf("================================================\n");
}

/* Requirement 6: performance & analytics report */
void generateReports(void) {
    int i, w;
    int countByUrgency[4] = {0, 0, 0, 0}; /* index 1,2,3 used; 0 unused */
    double totalRevenue = 0, totalDiscount = 0;
    int topPatientIdx = -1;
    double topAmount = -1;

    if (patientCount == 0) {
        printf("\nNo patients registered yet. Nothing to report.\n");
        return;
    }

    for (i = 0; i < patientCount; i++) {
        countByUrgency[patientUrgency[i]]++;
        totalRevenue  += patientFinalAmount[i];
        totalDiscount += patientDiscount[i];
        if (patientFinalAmount[i] > topAmount) {
            topAmount = patientFinalAmount[i];
            topPatientIdx = i;
        }
    }

    printf("\n============ PERFORMANCE REPORT ============\n");
    printf("Total Patients Registered : %d\n", patientCount);
    printf("  Level 1 (Normal)  : %d\n", countByUrgency[1]);
    printf("  Level 2 (Urgent)  : %d\n", countByUrgency[2]);
    printf("  Level 3 (Critical): %d\n", countByUrgency[3]);
    printf("\nTotal Revenue Earned      : LKR %.2f\n", totalRevenue);
    printf("Total Discounts Granted   : LKR %.2f\n", totalDiscount);

    printf("\nBed Occupancy Percentage per Ward:\n");
    for (w = 0; w < NUM_WARDS; w++) {
        int occupied = 0, b;
        for (b = 0; b < wardCapacity[w]; b++)
            if (bedOccupancy[w][b] == 1) occupied++;
        printf("  %-28s: %.1f%%\n", wardName[w], (occupied * 100.0) / wardCapacity[w]);
    }

    if (topPatientIdx != -1) {
        printf("\nHighest-Paying Patient    : %s (PAT-%d) - LKR %.2f\n",
               patientName[topPatientIdx], patientID[topPatientIdx], topAmount);
    }
    printf("================================================\n");
}

/* Requirement 7: load bed status from file if it exists, else start with all beds empty */
void loadBedStatus(void) {
    FILE *fp = fopen(BEDS_FILE, "r");
    int w, b, status;

    /* start clean first, in case the file is missing or incomplete */
    for (w = 0; w < NUM_WARDS; w++)
        for (b = 0; b < MAX_BEDS_PER_WARD; b++)
            bedOccupancy[w][b] = 0;

    if (fp == NULL) {
        printf("No previous bed status found. Starting fresh.\n");
        return;
    }
    for (w = 0; w < NUM_WARDS; w++) {
        for (b = 0; b < wardCapacity[w]; b++) {
            if (fscanf(fp, "%d", &status) == 1) {
                bedOccupancy[w][b] = status;
            }
        }
    }
    fclose(fp);
    printf("Bed status loaded from %s\n", BEDS_FILE);
}

/* Saves the current bed occupancy matrix to file so it survives program restarts */
void saveBedStatus(void) {
    FILE *fp = fopen(BEDS_FILE, "w");
    int w, b;

    if (fp == NULL) {
        printf("Error: could not save bed status.\n");
        return;
    }
    for (w = 0; w < NUM_WARDS; w++) {
        for (b = 0; b < wardCapacity[w]; b++) {
            fprintf(fp, "%d ", bedOccupancy[w][b]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

/* Appends a permanent billing log line for one patient - never overwrites old records */
void appendPatientRecord(int idx) {
    FILE *fp = fopen(RECORDS_FILE, "a");
    if (fp == NULL) {
        printf("Error: could not write patient record.\n");
        return;
    }
    fprintf(fp, "PAT-%d | %s | Age:%d | Urgency:%d | Specialty:%s | Final:LKR %.2f\n",
            patientID[idx], patientName[idx], patientAge[idx], patientUrgency[idx],
            specialtyName[patientSpecialtyIdx[idx]], patientFinalAmount[idx]);
    fclose(fp);
}

/* Loads the next patient ID to hand out, so IDs keep climbing across
   separate program runs instead of restarting at 1001 every time */
void loadNextPatientId(void) {
    FILE *fp = fopen(NEXT_ID_FILE, "r");
    int value;

    if (fp == NULL) {
        nextPatientId = 1001; /* first ever run */
        return;
    }
    if (fscanf(fp, "%d", &value) == 1) {
        nextPatientId = value;
    }
    fclose(fp);
}

/* Saves the next ID to hand out - called right after a new patient claims one */
void saveNextPatientId(void) {
    FILE *fp = fopen(NEXT_ID_FILE, "w");
    if (fp == NULL) {
        printf("Error: could not save next patient ID.\n");
        return;
    }
    fprintf(fp, "%d\n", nextPatientId);
    fclose(fp);
}

/* Requirement 2: patient intake, now using getValidatedInt() for every numeric field
   so a mis-typed letter can never crash or corrupt the registration anymore. */
void registerPatient(void) {
    int idx, specialtyChoice, wardChoice, admitted;

    if (patientCount >= MAX_PATIENTS) {
        printf("Patient limit reached. Cannot register more patients.\n");
        return;
    }
    idx = patientCount;
    patientID[idx] = nextPatientId;
    nextPatientId++;
    saveNextPatientId();

    printf("\n--- New Patient Registration ---\n");

    /* Name: keep re-prompting until something non-empty is typed */
    do {
        printf("Patient Name: ");
        fgets(patientName[idx], NAME_LEN, stdin);
        patientName[idx][strcspn(patientName[idx], "\n")] = '\0'; /* strip trailing newline */
        if (strlen(patientName[idx]) == 0)
            printf("Name cannot be empty.\n");
    } while (strlen(patientName[idx]) == 0);

    patientAge[idx]    = getValidatedInt("Patient Age (0-120): ", 0, 120);
    patientUrgency[idx] = getValidatedInt("Triage Level (1 = Normal, 2 = Urgent, 3 = Critical): ", 1, 3);

    printf("\nAvailable Specialties:\n");
    for (int i = 0; i < NUM_SPECIALTIES; i++)
        printf("  %d. %s (LKR %.2f)\n", i + 1, specialtyName[i], specialtyBaseFee[i]);
    specialtyChoice = getValidatedInt("Select Specialty ID (1-4): ", 1, NUM_SPECIALTIES);
    patientSpecialtyIdx[idx] = specialtyChoice - 1;

    admitted = getValidatedInt("Is Admitted to Ward? (1 = Yes, 0 = No): ", 0, 1);
    patientIsAdmitted[idx] = admitted;

    if (admitted == 1) {
        printf("\nAvailable Wards:\n");
        for (int i = 0; i < NUM_WARDS; i++)
            printf("  %d. %s (LKR %.2f/day)\n", i + 1, wardName[i], wardDailyRate[i]);
        wardChoice = getValidatedInt("Select Ward ID (1-4): ", 1, NUM_WARDS);
        patientWardIdx[idx] = wardChoice - 1;

        patientDaysAdmitted[idx] = getValidatedInt("Days Admitted (1-365): ", 1, 365);

        int bedIdx = assignBed(patientWardIdx[idx]);
        if (bedIdx == -1) {
            printf("\nSorry, %s is FULL. Patient registered as outpatient instead.\n",
                   wardName[patientWardIdx[idx]]);
            patientIsAdmitted[idx] = 0;
            patientWardIdx[idx] = -1;
            patientBedNumber[idx] = -1;
            patientDaysAdmitted[idx] = 0;
        } else {
            patientBedNumber[idx] = bedIdx;
        }
    } else {
        patientWardIdx[idx] = -1;
        patientBedNumber[idx] = -1;
        patientDaysAdmitted[idx] = 0;
    }

    patientWaitTime[idx]    = calculateWaitTime(patientSpecialtyIdx[idx]);
    patientBaseFee[idx]     = specialtyBaseFee[patientSpecialtyIdx[idx]];
    patientSurcharge[idx]   = calculateSurcharge(patientUrgency[idx], patientBaseFee[idx]);
    patientWardCost[idx]    = calculateWardCost(patientDaysAdmitted[idx], patientWardIdx[idx]);
    patientGrossTotal[idx]  = calculateGrossTotal(patientBaseFee[idx], patientSurcharge[idx], patientWardCost[idx]);
    patientDiscount[idx]    = calculateDiscount(patientAge[idx], patientGrossTotal[idx]);
    patientFinalAmount[idx] = calculateFinalAmount(patientGrossTotal[idx], patientDiscount[idx]);

    /* Increment queue count AFTER calculating wait time, per spec */
    specialtyQueueCount[patientSpecialtyIdx[idx]]++;
    patientCount++;

    printf("\n");
    displayBill(idx);

    appendPatientRecord(idx);
    saveBedStatus();
}
