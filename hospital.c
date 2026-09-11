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

/*  PATIENT ARRAYS  */
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

void showMainMenu(void) {
    printf("\n============ SMART HOSPITAL SYSTEM ============\n");
    printf("1. Register New Patient\n");
    printf("2. View Bed Occupancy Matrix\n");
    printf("3. View Patients by Priority (Triage Sort)\n");
    printf("4. Generate Performance Reports\n");
    printf("5. Save & Exit\n");
    printf("=================================================\n");
    printf("Enter your choice: ");
}
