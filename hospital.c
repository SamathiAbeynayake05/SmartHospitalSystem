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
